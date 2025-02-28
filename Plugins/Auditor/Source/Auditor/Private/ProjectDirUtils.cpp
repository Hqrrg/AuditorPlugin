// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectDirUtils.h"

#include "AuditorProjectSettings.h"
#include "Node.h"

namespace ProjectDirUtils
{
	bool CreateDirectory(Node* Tree)
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

	void GeneratePaths(Node* Root, FString& Path, TArray<FString>& Paths)
	{
		if (!Root) return;

		// Original root for the directory, i.e. the project folder
		if (!Root->GetParent()) Path.Append(Root->GetName() + "/");
	
		TArray<Node*> Children = Root->GetChildren();
		if (Children.IsEmpty()) return;
	
		for (Node* Child : Children)
		{
			FString RelativePath = Path;
		
			FString Name = Child->GetName();
			bool HasChildren = !Child->GetChildren().IsEmpty();
		
			RelativePath.Append(Name + (HasChildren ? "/" : ""));
		
			if (HasChildren) GeneratePaths(Child, RelativePath, Paths);
			else Paths.Add(RelativePath);
		}
	}

	Node* GetProjectDirectoryTree()
	{
		Node* ProjectDirectoryTree = new Node(nullptr, AuditorProjectSettings::GetProjectFolderName());
		
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

		return ProjectDirectoryTree;
	}
}
