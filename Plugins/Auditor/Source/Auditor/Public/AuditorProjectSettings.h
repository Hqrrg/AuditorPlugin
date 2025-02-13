// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuditedAsset.h"
#include "Engine/DeveloperSettings.h"
#include "AuditorProjectSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game, defaultconfig, meta = (DisplayName = "Auditor"))
class AUDITOR_API UAuditorProjectSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UAuditorProjectSettings();

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Naming Conventions", meta = (DisplayName = "Prefixes"))
	TMap<EAuditedAsset, FString> PrefixMap;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Naming Conventions", meta = (DisplayName = "Suffixes"))
	TMap<EAuditedAsset, FString> SuffixMap;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Naming Conventions", meta = (DisplayName = "Data Validation"))
	bool EnableDataValidation = true;

public:
	static bool GetPrefix(EAuditedAsset Key, FString& Prefix);
	static bool GetSuffix(EAuditedAsset Key, FString& Suffix);

private:
	void AddNamingConvention(EAuditedAsset Asset, FString Prefix = FString(TEXT("")), FString Suffix = FString(TEXT("")));
};
