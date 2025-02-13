// Fill out your copyright notice in the Description page of Project Settings.


#include "NamingConventionUtils.h"

#include "AuditorProjectSettings.h"


ENamingConventionTestResult NamingConventionUtils::CheckConformity(EAuditedAsset Key, FString Name)
{
	ENamingConventionTestResult Result = ENamingConventionTestResult::Conforms;
	
	FString Prefix;
	if (UAuditorProjectSettings::GetPrefix(Key, Prefix) && !Name.StartsWith(Prefix)) Result &= ~ENamingConventionTestResult::Prefix;

	FString Suffix;
	if (UAuditorProjectSettings::GetSuffix(Key, Suffix) && !Name.EndsWith(Suffix)) Result &= ~ENamingConventionTestResult::Suffix;

	return Result;
}
