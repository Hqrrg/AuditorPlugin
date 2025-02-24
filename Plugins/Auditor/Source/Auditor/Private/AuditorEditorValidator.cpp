// Fill out your copyright notice in the Description page of Project Settings.


#include "AuditorEditorValidator.h"

#include "AuditorProjectSettings.h"
#include "NamingConventionUtils.h"
#include "EditorValidatorSubsystem.h"
#include "Editor.h"

UAuditorEditorValidator::UAuditorEditorValidator()
{
	UEditorValidatorSubsystem* EditorValidatorSubsystem = GEditor->GetEditorSubsystem<UEditorValidatorSubsystem>();

	if (EditorValidatorSubsystem)
	{
		EditorValidatorSubsystem->AddValidator(this);
	}
}

bool UAuditorEditorValidator::CanValidateAsset_Implementation(const FAssetData& InAssetData, UObject* InObject, FDataValidationContext& InContext) const
{
	bool CanValidate = Super::CanValidateAsset_Implementation(InAssetData, InObject, InContext);

	FString ProjectFolderName = UAuditorProjectSettings::GetProjectFolderName();
	FString AssetPath = InAssetData.GetObjectPathString();

	TArray<FString> Folders;
	AssetPath.ParseIntoArray(Folders, TEXT("/"), true);

	bool InProjectFolder = false;

	if (!Folders.IsEmpty() && !ProjectFolderName.IsEmpty())
	{
		for (FString Folder : Folders)
		{
			InProjectFolder = Folder.Equals(ProjectFolderName);
			if (InProjectFolder) break;
		}
	}

	if (UAuditorProjectSettings::IsDataValidationEnabled() && InProjectFolder)
	{
		EAuditedAsset AssetKey = UNamingConventionUtils::GetAuditedAssetByClass(InAssetData.GetClass(), InAssetData);
		CanValidate = AssetKey != EAuditedAsset::None;
	}

	return CanValidate;
}

EDataValidationResult UAuditorEditorValidator::ValidateLoadedAsset_Implementation(const FAssetData& InAssetData, UObject* InAsset, FDataValidationContext& Context)
{
	EDataValidationResult Result;

	EAuditedAsset AssetKey = UNamingConventionUtils::GetAuditedAssetByClass(InAsset->GetClass(), InAssetData);
	ENamingConventionTestResult Conformity = UNamingConventionUtils::CheckConformity(AssetKey, InAsset->GetName());

	FString Prefix; UAuditorProjectSettings::GetPrefix(AssetKey, Prefix);
	FString Suffix; UAuditorProjectSettings::GetSuffix(AssetKey, Suffix);

	FString NoneErrorStr = FString(TEXT("Assets of type \'{Asset}\' must always be prefixed with \'{Prefix}\' and suffixed with \'{Suffix}\'."));
	FString PrefixErrorStr = FString(TEXT("Assets of type \'{Asset}\' must always be prefixed with \'{Prefix}\'."));
	FString SuffixErrorStr = FString(TEXT("Assets of type \'{Asset}\' must always be suffixed with \'{Suffix}\'."));

	FText NoneError = FText::FromString(NoneErrorStr);
	FText PrefixError = FText::FromString(PrefixErrorStr);
	FText SuffixError = FText::FromString(SuffixErrorStr);

	// Choose name to use for class
	FString NativeParentClassName = UNamingConventionUtils::GetAssetNativeParentClassName(InAssetData);
	FString AssetClassName = NativeParentClassName.IsEmpty() ? InAssetData.GetClass()->GetName() : NativeParentClassName;
	
	FFormatNamedArguments NoneErrorFormatArgs;
	NoneErrorFormatArgs.Add("Asset", FText::FromString(AssetClassName));
	NoneErrorFormatArgs.Add("Prefix", FText::FromString(Prefix));
	NoneErrorFormatArgs.Add("Suffix", FText::FromString(Suffix));

	FText FormattedNoneError = FText::Format(NoneError, NoneErrorFormatArgs);

	FFormatNamedArguments PrefixErrorFormatArgs;
	PrefixErrorFormatArgs.Add("Asset", FText::FromString(AssetClassName));
	PrefixErrorFormatArgs.Add("Prefix", FText::FromString(Prefix));

	FText FormattedPrefixError = FText::Format(PrefixError, PrefixErrorFormatArgs);
	
	FFormatNamedArguments SuffixErrorFormatArgs;
	SuffixErrorFormatArgs.Add("Asset", FText::FromString(AssetClassName));
	SuffixErrorFormatArgs.Add("Suffix", FText::FromString(Suffix));

	FText FormattedSuffixError = FText::Format(SuffixError, SuffixErrorFormatArgs);
	
	switch (Conformity)
	{
	case ENamingConventionTestResult::None:
			Result = EDataValidationResult::Invalid;
			AssetFails(InAsset, FormattedNoneError);
			break;
		
		case ENamingConventionTestResult::Prefix:
			Result = EDataValidationResult::Invalid;
			AssetFails(InAsset, FormattedSuffixError);
			break;
		
		case ENamingConventionTestResult::Suffix:
			Result = EDataValidationResult::Invalid;
			AssetFails(InAsset, FormattedPrefixError);
			break;
	default:
		Result = EDataValidationResult::Valid;
		AssetPasses(InAsset);
		break;
	}
	
	return Result;
}
