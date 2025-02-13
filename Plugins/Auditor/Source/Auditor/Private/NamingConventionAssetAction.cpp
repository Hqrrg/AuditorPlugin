// Fill out your copyright notice in the Description page of Project Settings.


#include "NamingConventionAssetAction.h"

#include "AuditorProjectSettings.h"
#include "EditorUtilityLibrary.h"
#include "LevelSequence.h"
#include "NamingConventionTestResult.h"
#include "NamingConventionUtils.h"
#include "WidgetBlueprint.h"
#include "Engine/UserDefinedEnum.h"
#include "Engine/UserDefinedStruct.h"
#include "Materials/MaterialInstanceConstant.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "NiagaraEmitter.h"
#include "NiagaraSystem.h"

UNamingConventionAssetAction::UNamingConventionAssetAction()
{
	// Materials
	SetupAsset(UTexture2D::StaticClass(), EAuditedAsset::Texture);
	SetupAsset(UMaterial::StaticClass(), EAuditedAsset::Material);
	SetupAsset(UMaterialInstanceConstant::StaticClass(), EAuditedAsset::MaterialInstance);
	// Physics
	SetupAsset(UPhysicsAsset::StaticClass(), EAuditedAsset::PhysicsAsset);
	SetupAsset(UPhysicalMaterial::StaticClass(), EAuditedAsset::PhysicalMaterial);
	// Meshes
	SetupAsset(USkeletalMesh::StaticClass(), EAuditedAsset::SkeletalMesh);
	SetupAsset(UStaticMesh::StaticClass(), EAuditedAsset::StaticMesh);
	// Blueprints
	SetupAsset(UBlueprint::StaticClass(), EAuditedAsset::Blueprint);
	SetupAsset(UWidgetBlueprint::StaticClass(), EAuditedAsset::Widget);
	SetupAsset(nullptr, EAuditedAsset::ActorComponent);
	SetupAsset(UAnimBlueprint::StaticClass(), EAuditedAsset::AnimationBlueprint);
	SetupAsset(nullptr, EAuditedAsset::BlueprintInterface);
	// Data
	SetupAsset(UCurveTable::StaticClass(), EAuditedAsset::CurveTable);
	SetupAsset(UDataTable::StaticClass(), EAuditedAsset::DataTable);
	SetupAsset(UUserDefinedEnum::StaticClass(), EAuditedAsset::Enum);
	SetupAsset(UUserDefinedStruct::StaticClass(), EAuditedAsset::Structure);
	// Niagara
	SetupAsset(UNiagaraEmitter::StaticClass(), EAuditedAsset::NiagaraEmitter);
	SetupAsset(UNiagaraSystem::StaticClass(), EAuditedAsset::NiagaraSystem);
	SetupAsset(UNiagaraScript::StaticClass(), EAuditedAsset::NiagaraFunction);
	// Skeletal Animation
	//AddSupportedClass(URig::StaticClass(), EAssetKey::Rig);
	SetupAsset(USkeleton::StaticClass(), EAuditedAsset::Skeleton);
	SetupAsset(UAnimMontage::StaticClass(), EAuditedAsset::AnimationMontage);
	SetupAsset(UAnimSequence::StaticClass(), EAuditedAsset::AnimationSequence);
	SetupAsset(UBlendSpace::StaticClass(), EAuditedAsset::BlendSpace);
	// Animation
	SetupAsset(ULevelSequence::StaticClass(), EAuditedAsset::LevelSequence);
	// Media
}

void UNamingConventionAssetAction::ApplyNamingConvention()
{
	TArray<FAssetData> SelectedAssetDataArray = UEditorUtilityLibrary::GetSelectedAssetData();

	for (FAssetData& SelectedAssetData : SelectedAssetDataArray)
	{
		UObject* SelectedAsset = SelectedAssetData.GetAsset();	
		TSoftClassPtr<UObject> AssetClass = SelectedAsset->GetClass();
		FString AssetName = SelectedAsset->GetName();

		EAuditedAsset AssetKey = GetAssetKeyFromClass(AssetClass);

		// Force blueprint variations (Interfaces, ActorComponents..)
		FString NativeParentClassName;
		if (SelectedAssetData.GetTagValue("NativeParentClass", NativeParentClassName))
		{
			//Remove path & clean up string
			NativeParentClassName = NativeParentClassName.TrimChar('\'');
			
			int32 To = NativeParentClassName.Len();
			int32 From = NativeParentClassName.Find(".", ESearchCase::IgnoreCase, ESearchDir::FromEnd, To)+1;
			
			NativeParentClassName = NativeParentClassName.Right(To - From);
			
			// Interface
			if (NativeParentClassName == FString(TEXT("Interface"))) AssetKey = EAuditedAsset::BlueprintInterface;
			
			// ActorComponent
			else if (NativeParentClassName == FString(TEXT("ActorComponent"))) AssetKey = EAuditedAsset::ActorComponent;
		}
		
		if (AssetKey == EAuditedAsset::None) continue;
		
		FString FormattedAssetName = AssetName;
		FString Prefix; UAuditorProjectSettings::GetPrefix(AssetKey, Prefix);
		FString Suffix; UAuditorProjectSettings::GetSuffix(AssetKey, Suffix);

		ENamingConventionTestResult Conformity = NamingConventionUtils::CheckConformity(AssetKey, AssetName);
		
		switch (Conformity)
		{
		// Has neither prefix nor suffix
		case ENamingConventionTestResult::None:
			FormattedAssetName = Prefix + FormattedAssetName + Suffix;
			break;

		// Has prefix but no suffix
		case ENamingConventionTestResult::Prefix:
			FormattedAssetName = FormattedAssetName + Suffix;
			break;

		// Has suffix but no prefix
		case ENamingConventionTestResult::Suffix:
			FormattedAssetName = Prefix + FormattedAssetName;
			break;

		// Conforms
		default:
			continue;
		}

		UEditorUtilityLibrary::RenameAsset(SelectedAsset, FormattedAssetName);
	}
}

EAuditedAsset UNamingConventionAssetAction::GetAssetKeyFromClass(TSoftClassPtr<UObject> Class)
{
	EAuditedAsset Key = EAuditedAsset::None;

	if (Class.IsValid() && AssetKeyMap.Contains(Class.Get()))
	{
		Key = AssetKeyMap[Class.Get()];
	}
	return Key;
}

// Set naming conventions for asset and add to registry
void UNamingConventionAssetAction::SetupAsset(TSoftClassPtr<UObject> Class, EAuditedAsset Key)
{
	// Optionally add to supported classes for this action
	if (Class.IsValid())
	{
		SupportedClasses.Add(Class.Get());
		AssetKeyMap.Add(Class.Get(), Key);
	}
}
