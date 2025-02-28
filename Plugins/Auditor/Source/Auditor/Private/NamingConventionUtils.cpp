// Fill out your copyright notice in the Description page of Project Settings.


#include "NamingConventionUtils.h"

#include "AuditedAsset.h"
#include "AuditorProjectSettings.h"
#include "LevelSequence.h"
#include "NamingConventionTestResult.h"
#include "NiagaraEmitter.h"
#include "NiagaraSystem.h"
#include "WidgetBlueprint.h"
#include "Engine/UserDefinedEnum.h"
#include "Engine/UserDefinedStruct.h"
#include "PhysicsEngine/PhysicsAsset.h"

namespace NamingConventionUtils
{
	namespace
	{
		TMap<TSoftClassPtr<UObject>, EAuditedAsset> AuditedAssetMap =
		{
			{ UTexture::StaticClass(), EAuditedAsset::Texture },
			{ UMaterial::StaticClass(), EAuditedAsset::Material },
			{ UMaterialInstance::StaticClass(), EAuditedAsset::MaterialInstance },
			{ UPhysicalMaterial::StaticClass(), EAuditedAsset::PhysicalMaterial },
			{ UPhysicsAsset::StaticClass(), EAuditedAsset::PhysicsAsset },
			{ USkeletalMesh::StaticClass(), EAuditedAsset::SkeletalMesh },
			{ UStaticMesh::StaticClass(), EAuditedAsset::StaticMesh },
			{ UBlueprint::StaticClass(), EAuditedAsset::Blueprint },
			{ UWidgetBlueprint::StaticClass(), EAuditedAsset::Widget },
			{ UCurveTable::StaticClass(), EAuditedAsset::CurveTable },
			{ UDataTable::StaticClass(), EAuditedAsset::DataTable },
			{ UUserDefinedEnum::StaticClass(), EAuditedAsset::Enum },
			{ UUserDefinedStruct::StaticClass(), EAuditedAsset::Structure },
			{ UNiagaraEmitter::StaticClass(), EAuditedAsset::NiagaraEmitter },
			{ UNiagaraSystem::StaticClass(), EAuditedAsset::NiagaraSystem },
			{ UNiagaraScript::StaticClass(), EAuditedAsset::NiagaraFunction },
			{ USkeleton::StaticClass(), EAuditedAsset::Skeleton },
			{ UAnimMontage::StaticClass(), EAuditedAsset::AnimationMontage },
			{ UAnimSequence::StaticClass(), EAuditedAsset::AnimationSequence },
			{ UBlendSpace::StaticClass(), EAuditedAsset::BlendSpace },
			{ ULevelSequence::StaticClass(), EAuditedAsset::LevelSequence }
		};
	}
	
	ENamingConventionTestResult CheckConformity(EAuditedAsset Key, FString Name)
	{
		ENamingConventionTestResult Result = ENamingConventionTestResult::Conforms;
		
		FString Prefix;
		if (AuditorProjectSettings::GetPrefix(Key, Prefix) && !Name.StartsWith(Prefix)) Result &= ~ENamingConventionTestResult::Prefix;

		FString Suffix;
		if (AuditorProjectSettings::GetSuffix(Key, Suffix) && !Name.EndsWith(Suffix)) Result &= ~ENamingConventionTestResult::Suffix;

		return Result;
	}

	EAuditedAsset GetAuditedAssetByClass(TSoftClassPtr<UObject> Class, const FAssetData& AssetData)
	{
		TMap<TSoftClassPtr<UObject>, EAuditedAsset>& AssetMap = GetAuditedAssetMap();
		
		EAuditedAsset AuditedAsset = EAuditedAsset::None;

		if (Class.IsValid() && AssetMap.Contains(Class.Get()))
		{
			AuditedAsset = AssetMap[Class.Get()];

			// Force blueprint variations (Interfaces, ActorComponents..)
			FString NativeParentClassName = NamingConventionUtils::GetAssetNativeParentClassName(AssetData);
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

	FString GetAssetNativeParentClassName(const FAssetData& AssetData)
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
	
	TMap<TSoftClassPtr<UObject>, EAuditedAsset>& GetAuditedAssetMap()
	{
		return AuditedAssetMap;
	}
}
