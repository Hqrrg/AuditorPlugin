// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuditedAsset.h"
#include "NamingConventionTestResult.h"

/**
 * 
 */
class AUDITOR_API NamingConventionUtils
{
	
public:
	static ENamingConventionTestResult CheckConformity(EAuditedAsset Key, FString Name);
};
