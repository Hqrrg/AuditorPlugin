// Fill out your copyright notice in the Description page of Project Settings.


#include "AuditorEditorValidator.h"

bool UAuditorEditorValidator::CanValidateAsset_Implementation(const FAssetData& InAssetData, UObject* InObject, FDataValidationContext& InContext) const
{
	bool CanValidate = Super::CanValidateAsset_Implementation(InAssetData, InObject, InContext);

	return CanValidate;
}

EDataValidationResult UAuditorEditorValidator::ValidateLoadedAsset_Implementation(const FAssetData& InAssetData, UObject* InAsset, FDataValidationContext& Context)
{
	EDataValidationResult Result =  Super::ValidateLoadedAsset_Implementation(InAssetData, InAsset, Context);

	return Result;
}
