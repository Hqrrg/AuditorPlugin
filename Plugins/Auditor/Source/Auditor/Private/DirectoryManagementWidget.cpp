// Fill out your copyright notice in the Description page of Project Settings.


#include "DirectoryManagementWidget.h"

#include "AuditorProjectSettings.h"
#include "DebugUtils.h"
#include "ProjectDirUtils.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"

SDirectoryManagementWidget::SDirectoryManagementWidget()
{
	ProjectFolderName = "";
	DataValidationStatus = false;
	Updated = false;
}

void SDirectoryManagementWidget::Construct(const FArguments& Args)
{
	bCanSupportFocus = true;

	ProjectFolderName = Args._ProjectFolderName;
	DataValidationStatus = Args._DataValidationStatus;

	ChildSlot
	[
		SNew(SConstraintCanvas)
		
		+SConstraintCanvas::Slot()
		.Anchors(FAnchors(.5f))
		.Offset(FVector2D(0.f, 0.f))
		.Alignment(FVector2D(.5f, .5f))
		.AutoSize(true)
		[
			SNew(SScaleBox)
			.Stretch(EStretch::ScaleToFit)
			.StretchDirection(EStretchDirection::Both)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SBox)
				.MinDesiredWidth(600.f)
				[
					SNew(SBorder)
					.Padding(FMargin(25.0f, 25.0f))
					[
						SNew(SVerticalBox)
						
						+SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(SUniformGridPanel)
							.SlotPadding(FMargin(5.f, 2.5f, 5.f, 2.5f))
							// Project Folder Title
							+SUniformGridPanel::Slot(0, 0)
							[
								SNew(STextBlock)
								.Text(FText::FromString("Project Folder:"))
							]
							// Project Folder Val
							+SUniformGridPanel::Slot(1, 0)
							[
								SNew(SBox)
								.WidthOverride(200.0f)
								[
									SNew(SEditableTextBox)
									.Text(FText::FromString(ProjectFolderName))
									.OverflowPolicy(ETextOverflowPolicy::Clip)
									.OnTextChanged(this, &SDirectoryManagementWidget::ProjectFolderNameChanged)
								]
							]
							// Data Validation Title
							+SUniformGridPanel::Slot(0, 1)
							[
								SNew(STextBlock)
								.Text(FText::FromString("Data Validation:"))
							]
							// Data Validation Val
							+SUniformGridPanel::Slot(1, 1)
							[
								SNew(SCheckBox)
								.IsChecked(DataValidationStatus)
								.OnCheckStateChanged(this, &SDirectoryManagementWidget::DataValidationStatusChanged)
							]
							// Update Button
							+SUniformGridPanel::Slot(0, 2)
							[
								SNew(SButton)
								.Text(FText::FromString("Update"))
								.OnClicked(this, &SDirectoryManagementWidget::UpdateButtonClicked)
									.HAlign(HAlign_Center)
									.VAlign(VAlign_Center)
							]
							// Cancel Button
							+SUniformGridPanel::Slot(1, 2)
							[
								SNew(SButton)
								.Text(FText::FromString("Cancel"))
								.OnClicked(this, &SDirectoryManagementWidget::CancelButtonClicked)
									.HAlign(HAlign_Center)
									.VAlign(VAlign_Center)
							]
						]
						// Create Project Directory Button
						+SVerticalBox::Slot()
						.AutoHeight()
						.Padding(FMargin(100.0f, 25.0f, 100.0f, 0.0f))
						[
							SNew(SButton)
							.Text(FText::FromString("Create Project Directory"))
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
							.OnClicked(this, &SDirectoryManagementWidget::CreateDirectoryButtonClicked)
						]
					]
				]
			]
		]
	];
}

void SDirectoryManagementWidget::ProjectFolderNameChanged(const FText& NewText)
{
	ProjectFolderName = NewText.ToString();
	Updated = true;
}

void SDirectoryManagementWidget::DataValidationStatusChanged(ECheckBoxState NewState)
{
	DataValidationStatus = NewState == ECheckBoxState::Checked;
	Updated = true;
}

FReply SDirectoryManagementWidget::UpdateButtonClicked()
{
	if (Updated)
	{
		FString OldProjectFolderName = AuditorProjectSettings::GetProjectFolderName();
		bool OldDataValidationStatus = AuditorProjectSettings::IsDataValidationEnabled();

		bool IsProjectFolderNameChanged = OldProjectFolderName != ProjectFolderName;
		bool IsDataValidationStatusChanged = OldDataValidationStatus != DataValidationStatus;

		FString ProjectFolderUpdatedStr = "Project Folder: " + OldProjectFolderName + " -> " + ProjectFolderName;
		FString DataValidationStatusUpdatedStr = FString::Printf(
		TEXT("Data Validation: %s -> %s"),
		OldDataValidationStatus ? TEXT("Enabled") : TEXT("Disabled"),
		DataValidationStatus ? TEXT("Enabled") : TEXT("Disabled"));
		
		EAppReturnType::Type Reply = DebugUtils::Message(EAppMsgType::YesNoCancel, TEXT("Confirm"),
			TEXT("Are you sure you want to save these changes?\n\n" +
				(IsProjectFolderNameChanged ? ProjectFolderUpdatedStr + TEXT("\n") : TEXT("")) +
				(IsDataValidationStatusChanged ? DataValidationStatusUpdatedStr : TEXT(""))));

		if (Reply == EAppReturnType::Yes)
		{
			if (IsProjectFolderNameChanged) AuditorProjectSettings::UpdateProjectFolderName(ProjectFolderName);
			if (IsDataValidationStatusChanged) AuditorProjectSettings::UpdateDataValidationEnabled(DataValidationStatus);
			RequestClose();
		}
		else if (Reply == EAppReturnType::No)
		{
			RequestClose();
		}
	}
	else
	{
		DebugUtils::Message(EAppMsgType::Ok, TEXT("Error"), TEXT("There are no changes to be saved."));
	}
	
	return FReply::Handled();
}

FReply SDirectoryManagementWidget::CancelButtonClicked()
{
	RequestClose();
	return FReply::Handled();
}

FReply SDirectoryManagementWidget::CreateDirectoryButtonClicked()
{
	// Save before creating directory
	AuditorProjectSettings::UpdateProjectFolderName(ProjectFolderName);
	AuditorProjectSettings::UpdateDataValidationEnabled(DataValidationStatus);
	
	if (ProjectFolderName.IsEmpty())
	{
		DebugUtils::Message(EAppMsgType::Ok, TEXT("Error"), TEXT("You must set a project folder name before you can perform this action."));
		return FReply::Handled();
	}
	
	bool Created = ProjectDirUtils::CreateDirectory(ProjectDirUtils::GetProjectDirectoryTree());

	if (Created)
	{
		DebugUtils::Message(EAppMsgType::Ok, TEXT("Success"), TEXT("Project directory successfully created."));
	}
	else
	{
		DebugUtils::Message(EAppMsgType::Ok, TEXT("Error"), TEXT("Something went wrong trying to create the project directory."));
	}
	
	return FReply::Handled();
}

void SDirectoryManagementWidget::OnCloseRequested(const FDirectoryManagementCloseRequested& InCloseRequested)
{
	CloseRequested = InCloseRequested;
}

void SDirectoryManagementWidget::RequestClose()
{
	if (CloseRequested.IsBound()) CloseRequested.Execute();
}
