// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FAuditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:

#pragma region MenuExtensions
	void InitializeMenuExtenders();
	
	TSharedRef<FExtender> RegisterNamingConventionPathMenuExtension(const TArray<FString>& SelectedPaths);
	TSharedRef<FExtender> RegisterNamingConventionAssetMenuExtension(const TArray<FAssetData>& SelectedAssets);
	TSharedRef<FExtender> RegisterDirectoryManagementMenuExtension();

	void AddNamingConventionPathMenuEntry(class FMenuBuilder& MenuBuilder);
	void AddNamingConventionAssetMenuEntry(class FMenuBuilder& MenuBuilder);
	void AddDirectoryManagementToolbarEntry(class FToolBarBuilder& ToolbarBuilder);

	void NamingConventionPathMenuButtonClicked();
	void NamingConventionAssetMenuButtonClicked();
	void DirectoryManagementToolbarButtonClicked();

	TArray<FString> LastSelectedPaths;
	TArray<FAssetData> LastSelectedAssets;
#pragma endregion

#pragma region SlateWidgets
	void RegisterDirectoryManagementWidget();
	TSharedRef<SDockTab> OnSpawnDirectoryManagementTab(const FSpawnTabArgs& SpawnTabArgs);
	void InvokeDirectoryManagementWidget();
#pragma endregion
};
