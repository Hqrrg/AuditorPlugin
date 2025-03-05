// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "AuditorProjectSettings.generated.h"

enum class EAuditedAsset : uint8;

/**
 * 
 */
UCLASS(Config=Game, defaultconfig, meta = (DisplayName = "Auditor"))
class AUDITOR_API UAuditorProjectSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UAuditorProjectSettings();

private:
	void AddNamingConvention(EAuditedAsset Asset, FString Prefix = FString(TEXT("")), FString Suffix = FString(TEXT("")));
	
public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Naming Conventions", meta = (DisplayName = "Prefixes"))
	TMap<EAuditedAsset, FString> PrefixMap;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Naming Conventions", meta = (DisplayName = "Suffixes"))
	TMap<EAuditedAsset, FString> SuffixMap;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Naming Conventions", meta = (DisplayName = "Data Validation"))
	bool EnableDataValidation = false;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Naming Conventions", meta = (DisplayName = "Project Folder"))
	FString ProjectFolderName = TEXT("Project");

	UPROPERTY(Config)
	bool FirstLaunch = true;
};

namespace AuditorProjectSettings
{
	bool GetPrefix(EAuditedAsset Key, FString& Prefix);
	bool GetSuffix(EAuditedAsset Key, FString& Suffix);

	bool IsDataValidationEnabled();
	FString GetProjectFolderName();

	inline bool IsProjectFolderSet() { return !GetProjectFolderName().IsEmpty(); }

	void UpdateDataValidationEnabled(bool NewVal);
	void UpdateProjectFolderName(FString NewVal);

	bool IsFirstLaunch();
	void RegisterFirstLaunch();
}
