// Fill out your copyright notice in the Description page of Project Settings.


#include "AuditorStyle.h"

#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

FName FAuditorStyle::StyleSetName = FName("AuditorStyle");
TSharedPtr<FSlateStyleSet> FAuditorStyle::AuditorStyleSet = nullptr;
FAuditorStylePropertyRegistry FAuditorStyle::AuditorStylePropertyRegistry = FAuditorStylePropertyRegistry();

void FAuditorStyle::Init()
{
	if (!AuditorStyleSet.IsValid())
	{
		AuditorStyleSet = CreateStyleSet();
		SetIconPropertyInStyleSet(AuditorStylePropertyRegistry.NamingConvention, "IconNamingConvention.png");
		
		FSlateStyleRegistry::RegisterSlateStyle(*AuditorStyleSet);
	}
}

void FAuditorStyle::DeInit()
{
}

TSharedRef<FSlateStyleSet> FAuditorStyle::CreateStyleSet()
{
	TSharedRef<FSlateStyleSet> StyleSet = MakeShareable(new FSlateStyleSet(StyleSetName));
	return StyleSet;
}

bool FAuditorStyle::SetIconPropertyInStyleSet(const FName PropertyName, const FString& FileName)
{
	if (!AuditorStyleSet.IsValid()) return false;

	const FString ResourcesDirectory = IPluginManager::Get().FindPlugin("Auditor")->GetBaseDir() + "/Resources";
	AuditorStyleSet->SetContentRoot(ResourcesDirectory);
	
	const FVector2D Icon128x128 = FVector2D(128.f, 128.f);
	AuditorStyleSet->Set(PropertyName, new FSlateImageBrush(ResourcesDirectory + "/" + FileName,Icon128x128));
	
	return true;
}
