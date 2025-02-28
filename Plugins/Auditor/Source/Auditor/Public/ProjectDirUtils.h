// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class Node;

namespace ProjectDirUtils
{
	bool CreateDirectory(Node* Tree);
	void GeneratePaths(Node* Root, FString& Path, TArray<FString>& Paths);
	Node* GetProjectDirectoryTree();
}

