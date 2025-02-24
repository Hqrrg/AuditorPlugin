// Fill out your copyright notice in the Description page of Project Settings.


#include "NamingConventionUtils.h"

#include "AuditorProjectSettings.h"
#include "LevelSequence.h"
#include "NiagaraEmitter.h"
#include "NiagaraSystem.h"
#include "WidgetBlueprint.h"
#include "Engine/UserDefinedEnum.h"
#include "Engine/UserDefinedStruct.h"
#include "PhysicsEngine/PhysicsAsset.h"


UNamingConventionUtils::UNamingConventionUtils()
{
	AuditedAssetMap = TMap<TSoftClassPtr<UObject>, EAuditedAsset>();

	AuditedAssetMap.Add(UTexture::StaticClass(), EAuditedAsset::Texture);
	AuditedAssetMap.Add(UMaterial::StaticClass(), EAuditedAsset::Material);
	AuditedAssetMap.Add(UMaterialInstance::StaticClass(), EAuditedAsset::MaterialInstance);
	AuditedAssetMap.Add(UPhysicalMaterial::StaticClass(), EAuditedAsset::PhysicalMaterial);
	AuditedAssetMap.Add(UPhysicsAsset::StaticClass(), EAuditedAsset::PhysicsAsset);
	AuditedAssetMap.Add(USkeletalMesh::StaticClass(), EAuditedAsset::SkeletalMesh);
	AuditedAssetMap.Add(UStaticMesh::StaticClass(), EAuditedAsset::StaticMesh);
	AuditedAssetMap.Add(UBlueprint::StaticClass(), EAuditedAsset::Blueprint);
	AuditedAssetMap.Add(UWidgetBlueprint::StaticClass(), EAuditedAsset::Widget);
	AuditedAssetMap.Add(UAnimBlueprint::StaticClass(), EAuditedAsset::AnimationBlueprint);
	AuditedAssetMap.Add(UCurveTable::StaticClass(), EAuditedAsset::CurveTable);
	AuditedAssetMap.Add(UDataTable::StaticClass(), EAuditedAsset::DataTable);
	AuditedAssetMap.Add(UUserDefinedEnum::StaticClass(), EAuditedAsset::Enum);
	AuditedAssetMap.Add(UUserDefinedStruct::StaticClass(), EAuditedAsset::Structure);
	AuditedAssetMap.Add(UNiagaraEmitter::StaticClass(), EAuditedAsset::NiagaraEmitter);
	AuditedAssetMap.Add(UNiagaraSystem::StaticClass(), EAuditedAsset::NiagaraSystem);
	AuditedAssetMap.Add(UNiagaraScript::StaticClass(), EAuditedAsset::NiagaraFunction);
	AuditedAssetMap.Add(USkeleton::StaticClass(), EAuditedAsset::Skeleton);
	AuditedAssetMap.Add(UAnimMontage::StaticClass(), EAuditedAsset::AnimationMontage);
	AuditedAssetMap.Add(UAnimSequence::StaticClass(), EAuditedAsset::AnimationSequence);
	AuditedAssetMap.Add(UBlendSpace::StaticClass(), EAuditedAsset::BlendSpace);
	AuditedAssetMap.Add(ULevelSequence::StaticClass(), EAuditedAsset::LevelSequence);
}

ENamingConventionTestResult UNamingConventionUtils::CheckConformity(EAuditedAsset Key, FString Name)
{
	ENamingConventionTestResult Result = ENamingConventionTestResult::Conforms;
	
	FString Prefix;
	if (UAuditorProjectSettings::GetPrefix(Key, Prefix) && !Name.StartsWith(Prefix)) Result &= ~ENamingConventionTestResult::Prefix;

	FString Suffix;
	if (UAuditorProjectSettings::GetSuffix(Key, Suffix) && !Name.EndsWith(Suffix)) Result &= ~ENamingConventionTestResult::Suffix;

	return Result;
}

EAuditedAsset UNamingConventionUtils::GetAuditedAssetByClass(TSoftClassPtr<UObject> Class, const FAssetData& AssetData)
{
	TMap<TSoftClassPtr<UObject>, EAuditedAsset> AssetMap = UNamingConventionUtils::GetAuditedAssetMap();
	
	EAuditedAsset AuditedAsset = EAuditedAsset::None;

	if (Class.IsValid() && AssetMap.Contains(Class.Get()))
	{
		AuditedAsset = AssetMap[Class.Get()];

		// Force blueprint variations (Interfaces, ActorComponents..)
		FString NativeParentClassName = GetAssetNativeParentClassName(AssetData);
		if (AuditedAsset == EAuditedAsset::Blueprint && !NativeParentClassName.IsEmpty())
		{
			// Interface
			if (NativeParentClassName.Equals(TEXT("Interface"))) AuditedAsset = EAuditedAsset::BlueprintInterface;

			// Actor Component
			if (NativeParentClassName.Equals(TEXT("ActorComponent"))) AuditedAsset = EAuditedAsset::ActorComponent;
		}
	}
	
	return AuditedAsset;
}

FString UNamingConventionUtils::GetAssetNativeParentClassName(const FAssetData& AssetData)
{
	FString NativeParentClassName;
	if (AssetData.GetTagValue("NativeParentClass", NativeParentClassName))
	{
		//Remove path & clean up string
		NativeParentClassName = NativeParentClassName.TrimChar('\'');
				
		int32 To = NativeParentClassName.Len();
		int32 From = NativeParentClassName.Find(".", ESearchCase::IgnoreCase, ESearchDir::FromEnd, To)+1;
				
		NativeParentClassName = NativeParentClassName.Right(To - From);
	}

	return NativeParentClassName;
}

TMap<TSoftClassPtr<UObject>, EAuditedAsset> UNamingConventionUtils::GetAuditedAssetMap()
{
	const UNamingConventionUtils* Utils = GetDefault<UNamingConventionUtils>();
	return Utils->AuditedAssetMap;
}
