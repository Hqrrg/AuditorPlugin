// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuditedAsset.h"
#include "Editor/Blutility/Classes/AssetActionUtility.h"
#include "NamingConventionAssetAction.generated.h"

/**
 * 
 */
UCLASS()
class AUDITOR_API UNamingConventionAssetAction : public UAssetActionUtility
{
	GENERATED_BODY()

public:
	UNamingConventionAssetAction();

public:
	UFUNCTION(CallInEditor)
	void ApplyNamingConvention();

private:
	EAuditedAsset GetAssetKeyFromClass(TSoftClassPtr<UObject> Class);

	void SetupAsset(TSoftClassPtr<UObject> Class = nullptr, EAuditedAsset Key = EAuditedAsset::None);

private:
	TMap<TSoftClassPtr<UObject>, EAuditedAsset> AssetKeyMap;
};
