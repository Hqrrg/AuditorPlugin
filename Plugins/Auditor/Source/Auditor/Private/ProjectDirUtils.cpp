// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectDirUtils.h"

#include "AuditorProjectSettings.h"
#include "Node.h"

namespace ProjectDirUtils
{
	bool CreateDirectory(TSharedPtr<Node> Tree)
	{
		FString ProjectPath = "/Game/";
		TArray<FString> Paths; GeneratePaths(Tree, ProjectPath, Paths);

		for (FString Path : Paths)
		{
			FString AbsolutePath;
			if (!FPackageName::TryConvertLongPackageNameToFilename(Path, AbsolutePath)) return false;
			if (!IFileManager::Get().MakeDirectory(*AbsolutePath, true)) return false;
		}
		return true;
	}

	void GeneratePaths(TSharedPtr<Node> Root, FString& Path, TArray<FString>& Paths)
	{
		if (!Root) return;

		// Original root for the directory, i.e. the project folder
		if (!Root->GetParent()) Path.Append(Root->GetName() + "/");
	
		TArray<TSharedPtr<Node>> Children = Root->GetChildren();
		if (Children.IsEmpty()) return;
	
		for (TSharedPtr<Node> Child : Children)
		{
			FString RelativePath = Path;
		
			FString Name = Child->GetName();
			bool HasChildren = !Child->GetChildren().IsEmpty();
		
			RelativePath.Append(Name + (HasChildren ? "/" : ""));
		
			if (HasChildren) GeneratePaths(Child, RelativePath, Paths);
			else Paths.Add(RelativePath);
		}
	}

	TSharedPtr<Node> GetProjectDirectoryTree()
	{
		TSharedPtr<Node> ProjectDirectoryTree = MakeShareable(new Node(nullptr, AuditorProjectSettings::GetProjectFolderName()));
		
		TSharedPtr<Node> AI = MakeShareable(new Node(ProjectDirectoryTree, TEXT("AI")));
		TSharedPtr<Node> Audio = MakeShareable(new Node(ProjectDirectoryTree, TEXT("Audio")));
		TSharedPtr<Node> Core = MakeShareable(new Node(ProjectDirectoryTree, TEXT("Core")));
		TSharedPtr<Node> Characters = MakeShareable(new Node(ProjectDirectoryTree, TEXT("Characters")));
		TSharedPtr<Node> FX = MakeShareable(new Node(ProjectDirectoryTree, TEXT("FX")));
		TSharedPtr<Node> Maps = MakeShareable(new Node(ProjectDirectoryTree, TEXT("Maps")));
		TSharedPtr<Node> UI = MakeShareable(new Node(ProjectDirectoryTree, TEXT("UI")));
		TSharedPtr<Node> Actions = MakeShareable(new Node(ProjectDirectoryTree, TEXT("Actions")));
		TSharedPtr<Node> Expeditions = MakeShareable(new Node(ProjectDirectoryTree, TEXT("Expeditions")));
		
		ProjectDirectoryTree->AddChildren({AI, Audio,  Core, Characters, FX, Maps, UI, Actions, Expeditions});

		TSharedPtr<Node> CharacterAnimations = MakeShareable(new Node(Characters, TEXT("Animations")));
		Characters->AddChild(CharacterAnimations);

		TSharedPtr<Node> FXFlares = MakeShareable(new Node(FX, TEXT("Flares")));
		FX->AddChild(FXFlares);

		TSharedPtr<Node> MapsDev = MakeShareable(new Node(FX, TEXT("Dev")));
		Maps->AddChild(MapsDev);

		TSharedPtr<Node> UIMaterials = MakeShareable(new Node(FX, TEXT("Materials")));
		TSharedPtr<Node> UIFonts = MakeShareable(new Node(FX, TEXT("Fonts")));
		UI->AddChildren({UIMaterials, UIFonts});

		return ProjectDirectoryTree;
	}
}
