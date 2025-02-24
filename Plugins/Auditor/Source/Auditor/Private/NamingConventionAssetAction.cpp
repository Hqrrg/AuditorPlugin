// Fill out your copyright notice in the Description page of Project Settings.


#include "NamingConventionAssetAction.h"

#include "AuditorProjectSettings.h"
#include "NamingConventionUtils.h"
#include "EditorUtilityLibrary.h"
#include "NamingConventionTestResult.h"

UNamingConventionAssetAction::UNamingConventionAssetAction()
{
	TArray<TSoftClassPtr<UObject>> AuditedAssetClasses;
	UNamingConventionUtils::GetAuditedAssetMap().GenerateKeyArray(AuditedAssetClasses);
	
	for (TSoftClassPtr<UObject> Class : AuditedAssetClasses)
	{
		SupportedClasses.Add(Class);
	}
}

void UNamingConventionAssetAction::ApplyNamingConvention()
{
	TArray<FAssetData> SelectedAssetDataArray = UEditorUtilityLibrary::GetSelectedAssetData();

	for (FAssetData& SelectedAssetData : SelectedAssetDataArray)
	{
		UObject* SelectedAsset = SelectedAssetData.GetAsset();	
		TSoftClassPtr<UObject> AssetClass = SelectedAsset->GetClass();
		FString AssetName = SelectedAsset->GetName();

		EAuditedAsset AssetKey = UNamingConventionUtils::GetAuditedAssetByClass(AssetClass, SelectedAssetData);
		
		if (AssetKey == EAuditedAsset::None) continue;
		
		FString FormattedAssetName = AssetName;
		FString Prefix; UAuditorProjectSettings::GetPrefix(AssetKey, Prefix);
		FString Suffix; UAuditorProjectSettings::GetSuffix(AssetKey, Suffix);

		ENamingConventionTestResult Conformity = UNamingConventionUtils::CheckConformity(AssetKey, AssetName);
		
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
