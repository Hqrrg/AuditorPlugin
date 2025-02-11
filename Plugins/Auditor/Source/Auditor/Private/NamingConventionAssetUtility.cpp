// Fill out your copyright notice in the Description page of Project Settings.


#include "NamingConventionAssetUtility.h"

#include "EditorUtilityLibrary.h"
#include "LevelSequence.h"
#include "WidgetBlueprint.h"
#include "Engine/UserDefinedEnum.h"
#include "Engine/UserDefinedStruct.h"
#include "Materials/MaterialInstanceConstant.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "NiagaraEmitter.h"
#include "NiagaraSystem.h"

UNamingConventionAssetUtility::UNamingConventionAssetUtility()
{
	AssetPrefixMap = TMap<EAssetKey, FString>();
	AssetSuffixMap = TMap<EAssetKey, FString>();

	// Materials
	SetupAsset(UTexture2D::StaticClass(), EAssetKey::Texture, FString(TEXT("T_")));
	SetupAsset(UMaterial::StaticClass(), EAssetKey::Material, FString(TEXT("M_")));
	SetupAsset(UMaterialInstanceConstant::StaticClass(), EAssetKey::MaterialInstance, FString(TEXT("MI_")));
	// Physics
	SetupAsset(UPhysicsAsset::StaticClass(), EAssetKey::PhysicsAsset, FString(TEXT("PHYS_")));
	SetupAsset(UPhysicalMaterial::StaticClass(), EAssetKey::PhysicalMaterial, FString(TEXT("PM_")));
	// Meshes
	SetupAsset(USkeletalMesh::StaticClass(), EAssetKey::SkeletalMesh, FString(TEXT("SK_")));
	SetupAsset(UStaticMesh::StaticClass(), EAssetKey::StaticMesh, FString(TEXT("SM_")));
	// Blueprints
	SetupAsset(UBlueprint::StaticClass(), EAssetKey::Blueprint, FString(TEXT("BP_")));
	SetupAsset(UWidgetBlueprint::StaticClass(), EAssetKey::Widget, FString(TEXT("WBP_")));
	SetupAsset(UActorComponent::StaticClass(), EAssetKey::ActorComponent, FString(TEXT("AC_")), FString(TEXT("")), false);
	SetupAsset(UAnimBlueprint::StaticClass(), EAssetKey::AnimationBlueprint, FString(TEXT("ABP_")));
	SetupAsset(UInterface::StaticClass(), EAssetKey::BlueprintInterface, FString(TEXT("BI_")), FString(TEXT("")), false);
	// Data
	SetupAsset(UCurveTable::StaticClass(), EAssetKey::CurveTable, FString(TEXT("CT_")));
	SetupAsset(UDataTable::StaticClass(), EAssetKey::DataTable, FString(TEXT("DT_")));
	SetupAsset(UUserDefinedEnum::StaticClass(), EAssetKey::Enum, FString(TEXT("E_")));
	SetupAsset(UUserDefinedStruct::StaticClass(), EAssetKey::Structure, FString(TEXT("F_")));
	// Niagara
	SetupAsset(UNiagaraEmitter::StaticClass(), EAssetKey::NiagaraEmitter, FString(TEXT("FXE_")));
	SetupAsset(UNiagaraSystem::StaticClass(), EAssetKey::NiagaraSystem, FString(TEXT("FXS_")));
	SetupAsset(UNiagaraScript::StaticClass(), EAssetKey::NiagaraFunction, FString(TEXT("FXF_")));
	// Skeletal Animation
	//AddSupportedClass(URig::StaticClass(), EAssetKey::Rig, FString(TEXT("Rig_")));
	SetupAsset(USkeleton::StaticClass(), EAssetKey::Skeleton, FString(TEXT("SKEL_")));
	SetupAsset(UAnimMontage::StaticClass(), EAssetKey::AnimationMontage, FString(TEXT("AM_")));
	SetupAsset(UAnimSequence::StaticClass(), EAssetKey::AnimationSequence, FString(TEXT("AS_")));
	SetupAsset(UBlendSpace::StaticClass(), EAssetKey::BlendSpace, FString(TEXT("BS_")));
	// Animation
	SetupAsset(ULevelSequence::StaticClass(), EAssetKey::LevelSequence, FString(TEXT("LS_")));
	// Media
}

void UNamingConventionAssetUtility::ApplyNamingConvention()
{
	TArray<FAssetData> SelectedAssetDataArray = UEditorUtilityLibrary::GetSelectedAssetData();

	for (FAssetData& SelectedAssetData : SelectedAssetDataArray)
	{
		UObject* SelectedAsset = SelectedAssetData.GetAsset();	
		TSoftClassPtr<UObject> AssetClass = SelectedAsset->GetClass();
		FString AssetName = SelectedAsset->GetName();

		EAssetKey AssetKey = GetAssetKeyFromClass(AssetClass);

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
			if (NativeParentClassName == FString(TEXT("Interface"))) AssetKey = EAssetKey::BlueprintInterface;
			
			// ActorComponent
			else if (NativeParentClassName == FString(TEXT("ActorComponent"))) AssetKey = EAssetKey::ActorComponent;
		}
		
		if (AssetKey == EAssetKey::None) continue;
		
		FString FormattedAssetName = AssetName;
		FString Prefix; GetPrefix(AssetKey, Prefix);
		FString Suffix; GetSuffix(AssetKey, Suffix);

		ENamingConventionTestResult Conformity = CheckConformity(AssetKey, AssetName);
		
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

ENamingConventionTestResult UNamingConventionAssetUtility::CheckConformity(EAssetKey Key, FString Name)
{
	ENamingConventionTestResult Result = ENamingConventionTestResult::Conforms;
	
	FString Prefix;
	if (GetPrefix(Key, Prefix) && !Name.StartsWith(Prefix)) Result &= ~ENamingConventionTestResult::Prefix;

	FString Suffix;
	if (GetSuffix(Key, Suffix) && !Name.EndsWith(Suffix)) Result &= ~ENamingConventionTestResult::Suffix;

	return Result;
}

bool UNamingConventionAssetUtility::GetPrefix(EAssetKey Key, FString& Prefix)
{
	bool HasPrefix = false;
	
	if (AssetPrefixMap.Contains(Key))
	{
		Prefix = AssetPrefixMap[Key];
		HasPrefix = true;
	}
	return HasPrefix;
}

bool UNamingConventionAssetUtility::GetSuffix(EAssetKey Key, FString& Suffix)
{
	bool HasSuffix = false;
	
	if (AssetSuffixMap.Contains(Key))
	{
		Suffix = AssetSuffixMap[Key];
		HasSuffix = true;
	}
	return HasSuffix;
}

EAssetKey UNamingConventionAssetUtility::GetAssetKeyFromClass(TSoftClassPtr<UObject> Class)
{
	EAssetKey Key = EAssetKey::None;

	if (Class.IsValid() && AssetKeyMap.Contains(Class.Get()))
	{
		Key = AssetKeyMap[Class.Get()];
	}
	return Key;
}

// Set naming conventions for asset and add to registry
void UNamingConventionAssetUtility::SetupAsset(TSoftClassPtr<UObject> Class, EAssetKey Key, FString Prefix, FString Suffix, bool Supported)
{
	// Optionally add to supported classes for this action
	if (Supported) SupportedClasses.Add(Class.Get());
	
	AssetKeyMap.Add(Class.Get(), Key);
	
	if (!Prefix.IsEmpty()) AssetPrefixMap.Add(Key, Prefix);
	if (!Suffix.IsEmpty()) AssetSuffixMap.Add(Key, Suffix);
}
