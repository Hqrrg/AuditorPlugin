// Fill out your copyright notice in the Description page of Project Settings.


#include "AuditorProjectSettings.h"

#include "AuditedAsset.h"

UAuditorProjectSettings::UAuditorProjectSettings()
{
	PrefixMap = TMap<EAuditedAsset, FString>();
	SuffixMap = TMap<EAuditedAsset, FString>();

	// Defaults
	AddNamingConvention(EAuditedAsset::Texture, FString(TEXT("T_")));
	AddNamingConvention(EAuditedAsset::Material, FString(TEXT("M_")));
	AddNamingConvention(EAuditedAsset::MaterialInstance, FString(TEXT("MI_")));
	AddNamingConvention(EAuditedAsset::PhysicalMaterial, FString(TEXT("PM_")));
	AddNamingConvention(EAuditedAsset::PhysicsAsset, FString(TEXT("PHYS_")));
	AddNamingConvention(EAuditedAsset::SkeletalMesh, FString(TEXT("SK_")));
	AddNamingConvention(EAuditedAsset::StaticMesh, FString(TEXT("SM_")));
	AddNamingConvention(EAuditedAsset::Blueprint, FString(TEXT("BP_")));
	AddNamingConvention(EAuditedAsset::Widget, FString(TEXT("WBP_")));
	AddNamingConvention(EAuditedAsset::ActorComponent, FString(TEXT("AC_")));
	AddNamingConvention(EAuditedAsset::AnimationBlueprint, FString(TEXT("ABP_")));
	AddNamingConvention(EAuditedAsset::BlueprintInterface, FString(TEXT("BI_")));
	AddNamingConvention(EAuditedAsset::CurveTable, FString(TEXT("CT_")));
	AddNamingConvention(EAuditedAsset::DataTable, FString(TEXT("DT_")));
	AddNamingConvention(EAuditedAsset::Enum, FString(TEXT("E_")));
	AddNamingConvention(EAuditedAsset::Structure, FString(TEXT("F_")));
	AddNamingConvention(EAuditedAsset::NiagaraEmitter, FString(TEXT("FXE_")));
	AddNamingConvention(EAuditedAsset::NiagaraSystem, FString(TEXT("FXS_")));
	AddNamingConvention(EAuditedAsset::NiagaraFunction, FString(TEXT("FXF_")));
	AddNamingConvention(EAuditedAsset::Rig, FString(TEXT("Rig_")));
	AddNamingConvention(EAuditedAsset::Skeleton, FString(TEXT("SKEL_")));
	AddNamingConvention(EAuditedAsset::AnimationMontage, FString(TEXT("AM_")));
	AddNamingConvention(EAuditedAsset::AnimationSequence, FString(TEXT("AS_")));
	AddNamingConvention(EAuditedAsset::BlendSpace, FString(TEXT("BS_")));
	AddNamingConvention(EAuditedAsset::LevelSequence, FString(TEXT("LS_")));
}

void UAuditorProjectSettings::AddNamingConvention(EAuditedAsset Asset, FString Prefix, FString Suffix)
{
	if (!Prefix.IsEmpty()) PrefixMap.Add(Asset, Prefix);
	if (!Suffix.IsEmpty()) SuffixMap.Add(Asset, Suffix);
}

namespace AuditorProjectSettings
{
	bool GetPrefix(EAuditedAsset Key, FString& Prefix)
	{
		const UAuditorProjectSettings* Settings = GetDefault<UAuditorProjectSettings>();
		if (!Settings) return false;

		TMap<EAuditedAsset, FString> SettingsPrefixMap = Settings->PrefixMap;
		
		bool HasPrefix = false;
		
		if (SettingsPrefixMap.Contains(Key))
		{
			Prefix = SettingsPrefixMap[Key];
			HasPrefix = true;
		}
		return HasPrefix;
	}

	bool GetSuffix(EAuditedAsset Key, FString& Suffix)
	{
		const UAuditorProjectSettings* Settings = GetDefault<UAuditorProjectSettings>();
		if (!Settings) return false;

		TMap<EAuditedAsset, FString> SettingsSuffixMap = Settings->SuffixMap;
		
		bool HasSuffix = false;
		
		if (SettingsSuffixMap.Contains(Key))
		{
			Suffix = SettingsSuffixMap[Key];
			HasSuffix = true;
		}
		return HasSuffix;
	}

	bool IsDataValidationEnabled()
	{
		const UAuditorProjectSettings* Settings = GetDefault<UAuditorProjectSettings>();
		return Settings->EnableDataValidation;
	}

	FString GetProjectFolderName()
	{
		const UAuditorProjectSettings* Settings = GetDefault<UAuditorProjectSettings>();
		return Settings->ProjectFolderName;
	}

	void UpdateDataValidationEnabled(bool NewVal)
	{
		UAuditorProjectSettings* Settings = GetMutableDefault<UAuditorProjectSettings>();
		Settings->EnableDataValidation = NewVal;
		Settings->SaveConfig(CPF_Config, *Settings->GetDefaultConfigFilename());
	}

	void UpdateProjectFolderName(FString NewVal)
	{
		UAuditorProjectSettings* Settings = GetMutableDefault<UAuditorProjectSettings>();
		Settings->ProjectFolderName = NewVal;
		Settings->SaveConfig(CPF_Config, *Settings->GetDefaultConfigFilename());
	}

	bool IsFirstLaunch()
	{
		const UAuditorProjectSettings* Settings = GetDefault<UAuditorProjectSettings>();
		return Settings->FirstLaunch;
	}

	void RegisterFirstLaunch()
	{
		UAuditorProjectSettings* Settings = GetMutableDefault<UAuditorProjectSettings>();
		Settings->FirstLaunch = false;
		Settings->SaveConfig(CPF_Config, *Settings->GetDefaultConfigFilename());
	}
}
