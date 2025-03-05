// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Styling/SlateStyle.h"

struct FAuditorStylePropertyRegistry
{
	FName NamingConvention;

	FAuditorStylePropertyRegistry()
	{
		NamingConvention = FName("ContentBrowser.NamingConvention");
	}
};

class FAuditorStyle
{

public:
	static void Init();
	static void DeInit();

	static TSharedRef<FSlateStyleSet> CreateStyleSet();
	static bool SetIconPropertyInStyleSet(const FName PropertyName, const FString& FileName);

	static FName GetStyleSetName() { return StyleSetName; }

	static FAuditorStylePropertyRegistry GetAuditorIconPropertyRegistry() { return AuditorStylePropertyRegistry; }

private:
	static FName StyleSetName;
	static TSharedPtr<FSlateStyleSet> AuditorStyleSet;

	static FAuditorStylePropertyRegistry AuditorStylePropertyRegistry;
};