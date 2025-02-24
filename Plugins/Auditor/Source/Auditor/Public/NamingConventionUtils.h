// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuditedAsset.h"
#include "NamingConventionTestResult.h"
#include "NamingConventionUtils.generated.h"

/**
 * 
 */
UCLASS()
class AUDITOR_API UNamingConventionUtils : public UObject
{
	GENERATED_BODY()

public:
	UNamingConventionUtils();
	
public:
	static ENamingConventionTestResult CheckConformity(EAuditedAsset Key, FString Name);

	static EAuditedAsset GetAuditedAssetByClass(TSoftClassPtr<UObject> Class, const FAssetData& AssetData);

	static FString GetAssetNativeParentClassName(const FAssetData& AssetData);

	static TMap<TSoftClassPtr<UObject>, EAuditedAsset> GetAuditedAssetMap();
	
public:
	TMap<TSoftClassPtr<UObject>, EAuditedAsset> AuditedAssetMap;
};
