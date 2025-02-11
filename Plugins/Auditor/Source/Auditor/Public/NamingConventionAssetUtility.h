// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Editor/Blutility/Classes/AssetActionUtility.h"
#include "NamingConventionAssetUtility.generated.h"


UENUM(BlueprintType)
enum class EAssetKey : uint8
{
	None = 0,
	Texture,
	Material,
	MaterialInstance,
	PhysicsAsset,
	PhysicalMaterial,
	SkeletalMesh,
	StaticMesh,
	Blueprint,
	Widget,
	ActorComponent,
	AnimationBlueprint,
	BlueprintInterface,
	CurveTable,
	DataTable,
	Enum,
	Structure,
	NiagaraEmitter,
	NiagaraSystem,
	NiagaraFunction,
	Rig,
	Skeleton,
	AnimationMontage,
	AnimationSequence,
	BlendSpace,
	LevelSequence
};

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ENamingConventionTestResult : uint8
{
	None	= 0b00000000, //0
	Prefix	= 0b00000001, //1
	Suffix	= 0b00000010, //2

	Conforms = Prefix + Suffix //3
};
ENUM_CLASS_FLAGS(ENamingConventionTestResult);

/**
 * 
 */
UCLASS()
class AUDITOR_API UNamingConventionAssetUtility : public UAssetActionUtility
{
	GENERATED_BODY()

public:
	UNamingConventionAssetUtility();

public:
	UFUNCTION(CallInEditor)
	void ApplyNamingConvention();

private:
	ENamingConventionTestResult CheckConformity(EAssetKey Key, FString Name);

	bool GetPrefix(EAssetKey Key, FString& Prefix);
	bool GetSuffix(EAssetKey Key, FString& Suffix);

	EAssetKey GetAssetKeyFromClass(TSoftClassPtr<UObject> Class);

	void SetupAsset(TSoftClassPtr<UObject> Class, EAssetKey Key, FString Prefix = FString(TEXT("")), FString Suffix = FString(TEXT("")), bool Supported = true);

private:
	TMap<EAssetKey, FString> AssetPrefixMap;
	TMap<EAssetKey, FString> AssetSuffixMap;

	TMap<TSoftClassPtr<UObject>, EAssetKey> AssetKeyMap;
};
