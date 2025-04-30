// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class Node;

namespace ProjectDirUtils
{
	bool CreateDirectory(TSharedPtr<Node> Tree);
	void GeneratePaths(TSharedPtr<Node> Root, FString& Path, TArray<FString>& Paths);
	TSharedPtr<Node> GetProjectDirectoryTree();
}

