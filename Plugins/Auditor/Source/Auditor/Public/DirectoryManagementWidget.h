// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Widgets/SCompoundWidget.h"

DECLARE_DELEGATE(FDirectoryManagementCloseRequested)

class SDirectoryManagementWidget : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SDirectoryManagementWidget) {}
		
	SLATE_ARGUMENT(FString, ProjectFolderName);
	SLATE_ARGUMENT(bool, DataValidationStatus);
		
	SLATE_END_ARGS()

public:
	SDirectoryManagementWidget();

public:
	void Construct(const FArguments& Args);
	
	void OnCloseRequested(const FDirectoryManagementCloseRequested& InCloseRequested);

private:
	void ProjectFolderNameChanged(const FText& NewText);
	void DataValidationStatusChanged(ECheckBoxState NewState);
	FReply UpdateButtonClicked();
	FReply CancelButtonClicked();
	FReply CreateDirectoryButtonClicked();
	
	void RequestClose();

private:
	FDirectoryManagementCloseRequested CloseRequested;
	FString ProjectFolderName;
	bool DataValidationStatus;

	bool Updated;
};
