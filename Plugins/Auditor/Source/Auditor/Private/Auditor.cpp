// Copyright Epic Games, Inc. All Rights Reserved.

#include "Auditor.h"

#include "AuditedAsset.h"
#include "AuditorProjectSettings.h"
#include "ContentBrowserModule.h"
#include "DebugUtils.h"
#include "EditorUtilityLibrary.h"
#include "LevelEditor.h"
#include "NamingConventionTestResult.h"
#include "NamingConventionUtils.h"
#include "EditorAssetLibrary.h"

#define LOCTEXT_NAMESPACE "FAuditorModule"

void FAuditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	InitializeMenuExtenders();
	RegisterDirectoryManagementWidget();
	
	if (AuditorProjectSettings::IsFirstInit())
	{
		AuditorProjectSettings::RegisterFirstInit();
		InvokeDirectoryManagementWidget();
		
		/*Node* ProjectDirectoryTree = ProjectDirUtils::GetProjectDirectoryTree();
		ProjectDirUtils::CreateDirectory(ProjectDirectoryTree);*/
	}
}

void FAuditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

// Contains all the functions related to context menu extensions
#pragma region ContextMenuExtensions

void FAuditorModule::InitializeMenuExtenders()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	TArray<FContentBrowserMenuExtender_SelectedPaths>& PathViewContextMenuExtenders = ContentBrowserModule.GetAllPathViewContextMenuExtenders();

	FContentBrowserMenuExtender_SelectedPaths NamingConventionPathMenuExtenderDelegate;
	NamingConventionPathMenuExtenderDelegate.BindRaw(this, &FAuditorModule::RegisterNamingConventionPathMenuExtension);
	PathViewContextMenuExtenders.Add(NamingConventionPathMenuExtenderDelegate);

	TArray<FContentBrowserMenuExtender_SelectedAssets>& AssetContextMenuExtenders = ContentBrowserModule.GetAllAssetViewContextMenuExtenders();
	
	FContentBrowserMenuExtender_SelectedAssets NamingConventionAssetMenuExtenderDelegate;
	NamingConventionAssetMenuExtenderDelegate.BindRaw(this, &FAuditorModule::RegisterNamingConventionAssetMenuExtension);
	AssetContextMenuExtenders.Add(NamingConventionAssetMenuExtenderDelegate);

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	TSharedRef<FExtender> DirectoryManagementMenuExtension = RegisterDirectoryManagementMenuExtension();
	
	LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(DirectoryManagementMenuExtension);
}
/* Naming Convention Menu Extension */

TSharedRef<FExtender> FAuditorModule::RegisterNamingConventionPathMenuExtension(const TArray<FString>& SelectedPaths)
{
	TSharedRef<FExtender> NamingConventionMenuExtender = MakeShareable(new FExtender());

	if (SelectedPaths.Num() > 0)
	{
		NamingConventionMenuExtender->AddMenuExtension(
			FName("FolderContext"),
			EExtensionHook::Position::After,
			TSharedPtr<FUICommandList>(),
			FMenuExtensionDelegate::CreateRaw(this, &FAuditorModule::AddNamingConventionPathMenuEntry));
	}

	LastSelectedPaths = SelectedPaths;
	return NamingConventionMenuExtender;
}

void FAuditorModule::AddNamingConventionPathMenuEntry(class FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Apply Naming Conventions")),
		FText::FromString(TEXT("Rename relevant assets to comply with project naming conventions (i.e. prefixes & suffixes).")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FAuditorModule::NamingConventionPathMenuButtonClicked),
		FName("NamingConventions"));
}

void FAuditorModule::NamingConventionPathMenuButtonClicked()
{
	if (LastSelectedPaths.IsEmpty())
	{
		DebugUtils::LogError("[NamingConventions] Please select a folder to perform this action on.");
		return;
	}
	
	if (LastSelectedPaths.Num() > 1)
	{
		DebugUtils::Message(EAppMsgType::Type::Ok, "Error", "Please only select one folder to perform this action on.");
		return;
	}
	
	if (!LastSelectedPaths[0].Contains(AuditorProjectSettings::GetProjectFolderName()))
	{
		DebugUtils::Message(EAppMsgType::Type::Ok, "Error", "Please select a folder within the project directory to perform this action on.");
        return;
	}
	
	TArray<FString> AssetPaths = UEditorAssetLibrary::ListAssets(LastSelectedPaths[0]);

	if (AssetPaths.IsEmpty())
	{
		DebugUtils::Message(EAppMsgType::Ok, "Error", "No assets found in selected paths.");
		return;
	}

	int32 NumFormattedAssets = 0;

	for (FString& AssetPath : AssetPaths)
	{
		const FAssetData& AssetData = UEditorAssetLibrary::FindAssetData(AssetPath);
		UObject* Asset = AssetData.GetAsset();
		TSoftClassPtr<UObject> AssetClass = Asset->GetClass();
		FString AssetName = Asset->GetName();

		EAuditedAsset AssetKey = NamingConventionUtils::GetAuditedAssetByClass(AssetClass, AssetData);
		
		if (AssetKey == EAuditedAsset::None) continue;
		
		FString FormattedAssetName = AssetName;
		FString Prefix; AuditorProjectSettings::GetPrefix(AssetKey, Prefix);
		FString Suffix; AuditorProjectSettings::GetSuffix(AssetKey, Suffix);

		ENamingConventionTestResult Conformity = NamingConventionUtils::CheckConformity(AssetKey, AssetName);
		
		switch (Conformity)
		{
			// Has neither prefix nor suffix
		case ENamingConventionTestResult::None:
			FormattedAssetName = Prefix + FormattedAssetName + Suffix;
			break;

			// Has prefix but no suffix
		case ENamingConventionTestResult::Prefix:
			FormattedAssetName = FormattedAssetName + Suffix;
			break;

			// Has suffix but no prefix
		case ENamingConventionTestResult::Suffix:
			FormattedAssetName = Prefix + FormattedAssetName;
			break;

			// Conforms
		default:
			continue;
		}

		UEditorUtilityLibrary::RenameAsset(Asset, FormattedAssetName);
		NumFormattedAssets++;
	}
	
	if (NumFormattedAssets > 0)
	{
		DebugUtils::Notify(FString::Printf(TEXT("Formatted %d asset%s to conform to naming conventions."),
		NumFormattedAssets,
		NumFormattedAssets>1 ? TEXT("s") : TEXT("")),
		3.0f);
	}
	else DebugUtils::Notify("Found assets are already conforming to naming conventions.", 3.0f);
}

TSharedRef<FExtender> FAuditorModule::RegisterNamingConventionAssetMenuExtension(const TArray<FAssetData>& SelectedAssets)
{
	TSharedRef<FExtender> NamingConventionMenuExtender = MakeShareable(new FExtender());

	if (SelectedAssets.Num() > 0)
	{
		NamingConventionMenuExtender->AddMenuExtension(
			FName("Delete"),
			EExtensionHook::Position::After,
			TSharedPtr<FUICommandList>(),
			FMenuExtensionDelegate::CreateRaw(this, &FAuditorModule::AddNamingConventionAssetMenuEntry));
	}
	LastSelectedAssets = SelectedAssets;
	return NamingConventionMenuExtender;
}

void FAuditorModule::AddNamingConventionAssetMenuEntry(class FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Apply Naming Conventions")),
		FText::FromString(TEXT("Rename relevant assets to comply with project naming conventions (i.e. prefixes & suffixes).")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FAuditorModule::NamingConventionAssetMenuButtonClicked),
		FName("NamingConventions"));
}

void FAuditorModule::NamingConventionAssetMenuButtonClicked()
{
	if (LastSelectedAssets.IsEmpty())
	{
		DebugUtils::LogError("[NamingConventions] Please select an asset to perform this action on.");
		return;
	}
	
	if (!LastSelectedAssets[0].PackagePath.ToString().Contains(AuditorProjectSettings::GetProjectFolderName()))
	{
		DebugUtils::Message(EAppMsgType::Type::Ok, "Error", "Please select an asset within the project directory to perform this action on.");
		return;
	}
	
	int32 NumAssetsFormatted = 0;

	for (const FAssetData& AssetData : LastSelectedAssets)
	{
		UObject* Asset = AssetData.GetAsset();	
		TSoftClassPtr<UObject> AssetClass = Asset->GetClass();
		FString AssetName = Asset->GetName();

		EAuditedAsset AssetKey = NamingConventionUtils::GetAuditedAssetByClass(AssetClass, AssetData);
		
		if (AssetKey == EAuditedAsset::None) continue;
		
		FString FormattedAssetName = AssetName;
		FString Prefix; AuditorProjectSettings::GetPrefix(AssetKey, Prefix);
		FString Suffix; AuditorProjectSettings::GetSuffix(AssetKey, Suffix);

		ENamingConventionTestResult Conformity = NamingConventionUtils::CheckConformity(AssetKey, AssetName);
		
		switch (Conformity)
		{
			// Has neither prefix nor suffix
		case ENamingConventionTestResult::None:
			FormattedAssetName = Prefix + FormattedAssetName + Suffix;
			break;

			// Has prefix but no suffix
		case ENamingConventionTestResult::Prefix:
			FormattedAssetName = FormattedAssetName + Suffix;
			break;

			// Has suffix but no prefix
		case ENamingConventionTestResult::Suffix:
			FormattedAssetName = Prefix + FormattedAssetName;
			break;

			// Conforms
		default:
			continue;
		}

		UEditorUtilityLibrary::RenameAsset(Asset, FormattedAssetName);
		NumAssetsFormatted++;
	}

	if (NumAssetsFormatted > 0)
	{
		DebugUtils::Notify(FString::Printf(TEXT("Formatted %d asset%s to conform to naming conventions."),
		NumAssetsFormatted,
		NumAssetsFormatted>1 ? TEXT("s") : TEXT("")),
		3.0f);
	}
	else DebugUtils::Notify("Selected assets are already conforming to naming conventions.", 3.0f);
}

/* Directory Management Menu Extension */

TSharedRef<FExtender> FAuditorModule::RegisterDirectoryManagementMenuExtension()
{
	TSharedRef<FExtender> DirectoryManagementMenuExtender = MakeShareable(new FExtender());

	DirectoryManagementMenuExtender->AddToolBarExtension(
		FName("File"),
		EExtensionHook::Position::After,
		TSharedPtr<FUICommandList>(),
		FToolBarExtensionDelegate::CreateRaw(this, &FAuditorModule::AddDirectoryManagementToolbarEntry));
	
	return DirectoryManagementMenuExtender;
}

void FAuditorModule::AddDirectoryManagementToolbarEntry(class FToolBarBuilder& ToolbarBuilder)
{
	ToolbarBuilder.AddSeparator(FName("Auditor"));
	
	ToolbarBuilder.AddToolBarButton(
		FUIAction(FExecuteAction::CreateRaw(this, &FAuditorModule::DirectoryManagementToolbarButtonClicked)),
		NAME_None,
		FText::FromString(TEXT("Directory Management")),
		FText::FromString(TEXT("Open directory management settings.")),
			FSlateIcon());
}

void FAuditorModule::DirectoryManagementToolbarButtonClicked()
{
	InvokeDirectoryManagementWidget();
}

#pragma endregion

// Contains all the functions related to plugin slate widgets
#pragma region SlateWidgets

void FAuditorModule::RegisterDirectoryManagementWidget()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		FName("DirectoryManagement"),
		FOnSpawnTab::CreateRaw(this, &FAuditorModule::OnSpawnDirectoryManagementTab))
	.SetDisplayName(FText::FromString("Directory Management"));
}

TSharedRef<SDockTab> FAuditorModule::OnSpawnDirectoryManagementTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab).TabRole(ETabRole::NomadTab);
}

void FAuditorModule::InvokeDirectoryManagementWidget()
{
	FGlobalTabmanager::Get()->TryInvokeTab(FName("DirectoryManagement"));
}

#pragma endregion

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAuditorModule, Auditor)