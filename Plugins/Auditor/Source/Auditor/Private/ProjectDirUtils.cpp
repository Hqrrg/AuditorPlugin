// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectDirUtils.h"

bool UProjectDirUtils::CreateDirectory(Node* Tree)
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

void UProjectDirUtils::GeneratePaths(Node* Root, FString& Path, TArray<FString>& Paths)
{
	if (!Root) return;

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
