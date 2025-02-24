// Copyright Epic Games, Inc. All Rights Reserved.

#include "Auditor.h"

#include "AuditorProjectSettings.h"
#include "Node.h"
#include "ProjectDirUtils.h"

#define LOCTEXT_NAMESPACE "FAuditorModule"

void FAuditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	const UAuditorProjectSettings* Settings = GetDefault<UAuditorProjectSettings>();
	if (Settings && Settings->IsFirstInit())
	{
		Settings->RegisterFirstInit();

		Node* ProjectDirectoryTree = new Node(nullptr, UAuditorProjectSettings::GetProjectFolderName());

		Node* AI = new Node(ProjectDirectoryTree, TEXT("AI"));
		Node* Audio = new Node(ProjectDirectoryTree, TEXT("Audio"));
		Node* Core = new Node(ProjectDirectoryTree, TEXT("Core"));
		Node* Characters = new Node(ProjectDirectoryTree, TEXT("Characters"));
		Node* FX = new Node(ProjectDirectoryTree, TEXT("FX"));
		Node* Maps = new Node(ProjectDirectoryTree, TEXT("Maps"));
		Node* UI = new Node(ProjectDirectoryTree, TEXT("UI"));
		Node* Actions = new Node(ProjectDirectoryTree, TEXT("Actions"));
		Node* Expeditions = new Node(ProjectDirectoryTree, TEXT("Expeditions"));
		
		ProjectDirectoryTree->AddChildren({AI, Audio,  Core, Characters, FX, Maps, UI, Actions, Expeditions});

		Node* CharacterAnimations = new Node(Characters, TEXT("Animations"));
		Characters->AddChild(CharacterAnimations);

		Node* FXFlares = new Node(FX, TEXT("Flares"));
		FX->AddChild(FXFlares);

		Node* MapsDev = new Node(FX, TEXT("Dev"));
		Maps->AddChild(MapsDev);

		Node* UIMaterials = new Node(FX, TEXT("Materials"));
		Node* UIFonts = new Node(FX, TEXT("Fonts"));
		UI->AddChildren({UIMaterials, UIFonts});

		UProjectDirUtils::CreateDirectory(ProjectDirectoryTree);

		// Prompt user to create folder structure / set project folder
	}
}

void FAuditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAuditorModule, Auditor)