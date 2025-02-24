// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Node.h"
#include "UObject/Object.h"
#include "ProjectDirUtils.generated.h"

/**
 * 
 */
UCLASS()
class AUDITOR_API UProjectDirUtils : public UObject
{
	GENERATED_BODY()

public:
	static bool CreateDirectory(Node* Tree);

private:
	static void GeneratePaths(Node* Root, FString& Path, TArray<FString>& Paths);
};
