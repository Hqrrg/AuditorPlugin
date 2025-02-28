// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

enum class EAuditedAsset : uint8;
enum class ENamingConventionTestResult : uint8;

namespace NamingConventionUtils
{
	ENamingConventionTestResult CheckConformity(EAuditedAsset Key, FString Name);
	EAuditedAsset GetAuditedAssetByClass(TSoftClassPtr<UObject> Class, const FAssetData& AssetData);
	FString GetAssetNativeParentClassName(const FAssetData& AssetData);
	TMap<TSoftClassPtr<UObject>, EAuditedAsset>& GetAuditedAssetMap();
}
