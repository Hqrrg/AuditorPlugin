// Copyright Epic Games, Inc. All Rights Reserved.

#include "Auditor.h"

#include "AuditedAsset.h"
#include "AuditorProjectSettings.h"
#include "AuditorStyle.h"
#include "ContentBrowserModule.h"
#include "DebugUtils.h"
#include "DirectoryManagementWidget.h"
#include "EditorUtilityLibrary.h"
#include "LevelEditor.h"
#include "NamingConventionTestResult.h"
#include "NamingConventionUtils.h"
#include "EditorAssetLibrary.h"
#include "Node.h"
#include "ProjectDirUtils.h"
#include "Windows/WindowsApplication.h"

#define LOCTEXT_NAMESPACE "FAuditorModule"

void FAuditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FAuditorStyle::Init();
	InitializeMenuExtenders();
	RegisterDirectoryManagementWidget();
	
	if (AuditorProjectSettings::IsFirstLaunch())
	{
		AuditorProjectSettings::RegisterFirstLaunch();
		InvokeDirectoryManagementWidget();
	}
}

void FAuditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	FAuditorStyle::DeInit();
}

// Contains all the functions related to context menu extensions
#pragma region ContextMenuExtensions

void FAuditorModule::InitializeMenuExtenders()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	// Folder menu extenders
	TArray<FContentBrowserMenuExtender_SelectedPaths>& PathViewContextMenuExtenders = ContentBrowserModule.GetAllPathViewContextMenuExtenders();
	// Register naming convention folder menu extender
	FContentBrowserMenuExtender_SelectedPaths NamingConventionPathMenuExtenderDelegate;
	NamingConventionPathMenuExtenderDelegate.BindRaw(this, &FAuditorModule::RegisterNamingConventionPathMenuExtension);
	PathViewContextMenuExtenders.Add(NamingConventionPathMenuExtenderDelegate);
	// Asset menu extenders
	TArray<FContentBrowserMenuExtender_SelectedAssets>& AssetContextMenuExtenders = ContentBrowserModule.GetAllAssetViewContextMenuExtenders();
	// Register naming convention asset menu extender
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
		FSlateIcon(FAuditorStyle::GetStyleSetName(), FAuditorStyle::GetAuditorIconPropertyRegistry().NamingConvention),
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

	if (!AuditorProjectSettings::IsProjectFolderSet())
	{
		EAppReturnType::Type Reply = DebugUtils::Message(EAppMsgType::Type::YesNo, "Error",
			"You must assign a project folder before you can use this action.\nWould you like to assign one now?");

		if (Reply == EAppReturnType::Type::Yes) InvokeDirectoryManagementWidget();
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

	// Loop through all selected asset paths
	for (FString& AssetPath : AssetPaths)
	{
		// Extract data from asset at path
		const FAssetData& AssetData = UEditorAssetLibrary::FindAssetData(AssetPath);
		UObject* Asset = AssetData.GetAsset();
		TSoftClassPtr<UObject> AssetClass = Asset->GetClass();
		FString AssetName = Asset->GetName();

		// Get the asset key associated with the asset
		EAuditedAsset AssetKey = NamingConventionUtils::GetAuditedAssetByClass(AssetClass, AssetData);
		
		if (AssetKey == EAuditedAsset::None) continue;

		// Fetch naming convention information from plugin config
		FString FormattedAssetName = AssetName;
		FString Prefix; AuditorProjectSettings::GetPrefix(AssetKey, Prefix);
		FString Suffix; AuditorProjectSettings::GetSuffix(AssetKey, Suffix);

		ENamingConventionTestResult Conformity = NamingConventionUtils::CheckConformity(AssetKey, AssetName);

		// Assign naming conventions based on conformity of the current name
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

		// Rename the asset with the correct naming format
		UEditorUtilityLibrary::RenameAsset(Asset, FormattedAssetName);
		NumFormattedAssets++;
	}

	// Notify the user as to how many assets were affected
	if (NumFormattedAssets > 0)
	{
		DebugUtils::Notify(FString::Printf(TEXT("Formatted %d asset%s to conform to naming conventions."),
		NumFormattedAssets,
		NumFormattedAssets>1 ? TEXT("s") : TEXT("")),
		3.0f);
	}
	else DebugUtils::Notify("Found assets are already conforming to naming conventions.", 3.0f);
}

// Register the context menu extender for applying naming conventions
TSharedRef<FExtender> FAuditorModule::RegisterNamingConventionAssetMenuExtension(const TArray<FAssetData>& SelectedAssets)
{
	TSharedRef<FExtender> NamingConventionMenuExtender = MakeShareable(new FExtender());

	if (SelectedAssets.Num() > 0) // If there are assets selected
	{
		NamingConventionMenuExtender->AddMenuExtension(
			FName("Delete"), // Hook (Existing Menu Entry)
			EExtensionHook::Position::After, // Position (Place this entry after the hook)
			TSharedPtr<FUICommandList>(),
			// Calls function to add the menu entry
			FMenuExtensionDelegate::CreateRaw(this, &FAuditorModule::AddNamingConventionAssetMenuEntry)); 
	}
	LastSelectedAssets = SelectedAssets;
	return NamingConventionMenuExtender;
}

// Build the menu entry
void FAuditorModule::AddNamingConventionAssetMenuEntry(class FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Apply Naming Conventions")),
		FText::FromString(TEXT("Rename relevant assets to comply with project naming conventions (i.e. prefixes & suffixes).")),
		// Icon
		FSlateIcon(FAuditorStyle::GetStyleSetName(), FAuditorStyle::GetAuditorIconPropertyRegistry().NamingConvention),
		// Function to call when entry is clicked
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

	if (!AuditorProjectSettings::IsProjectFolderSet())
	{
		EAppReturnType::Type Reply = DebugUtils::Message(EAppMsgType::Type::YesNo, "Error",
			"You must assign a project folder before you can use this action.\nWould you like to assign one now?");

		if (Reply == EAppReturnType::Type::Yes) InvokeDirectoryManagementWidget();
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
	.SetDisplayName(FText::FromString("Directory Management"))
	.SetAutoGenerateMenuEntry(false);
}

TSharedRef<SDockTab> FAuditorModule::OnSpawnDirectoryManagementTab(const FSpawnTabArgs& SpawnTabArgs)
{
	// Creating and setting params
	TSharedRef<SDirectoryManagementWidget> DirectoryManagementWidget = SNew(SDirectoryManagementWidget)
		.DataValidationStatus(AuditorProjectSettings::IsDataValidationEnabled())
		.ProjectFolderName(AuditorProjectSettings::GetProjectFolderName());
	
	// Adding functionality to close button
	DirectoryManagementWidget->OnCloseRequested(FDirectoryManagementCloseRequested::CreateRaw(this, &FAuditorModule::CloseDirectoryManagementWidget));
	
	TSharedRef<SDockTab> Tab = SNew(SDockTab) // Creating a new nomad tab (floating tab)
	.TabRole(ETabRole::NomadTab)
	[
		DirectoryManagementWidget
	]
	.ContentPadding(FMargin(25.f));
	// Assign unique tab manager
	TSharedRef<FTabManager> TabManager = FGlobalTabmanager::Get()->NewTabManager(Tab); 
	Tab->SetTabManager(TabManager.ToSharedPtr());

	return Tab;
}

void FAuditorModule::InvokeDirectoryManagementWidget()
{
	TSharedPtr<SDockTab> Tab = FGlobalTabmanager::Get()->TryInvokeTab(FName("DirectoryManagement"));
	
	if (Tab.IsValid())
	{
		// We'll go with this for now even if not sure if it actually works. Stops tab from being docked though.
		if (TSharedPtr<FTabManager> TabManager = Tab->GetTabManagerPtr())
		{
			TabManager.Get()->SetCanDoDragOperation(false);
		}
		TSharedPtr<SWindow> Window = Tab->GetParentWindow();
		if (Window.IsValid())
		{
			Window.Get()->SetSizingRule(ESizingRule::Autosized);
		}
	}
}

void FAuditorModule::CloseDirectoryManagementWidget()
{
	TSharedPtr<SDockTab> Tab = FGlobalTabmanager::Get()->FindExistingLiveTab(FName("DirectoryManagement"));

	if (Tab.IsValid())
	{
		bool CloseRequest = Tab->RequestCloseTab();
		if (!CloseRequest) DebugUtils::LogError(TEXT("[Directory Management] Tab close request denied."));
	}
}

#pragma endregion

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAuditorModule, Auditor)