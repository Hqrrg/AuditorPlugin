#pragma once

#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Logging/LogVerbosity.h"

namespace DebugUtils
{
	inline void Print(const FString& Content, const FColor Colour = FColor::Yellow, const float Duration = 2.0f)
	{
		GEditor->AddOnScreenDebugMessage(-1, Duration, Colour, Content);
	}

	inline void Notify(const FString& Content, const float Duration)
	{
		FSlateNotificationManager& NotificationManager = FSlateNotificationManager::Get();

		FNotificationInfo NotificationInfo = FNotificationInfo((
			FText::FromString("DebugNotify_Notification"),
			FText::FromString(Content)));

		NotificationInfo.bUseLargeFont = true;
		
		NotificationInfo.ExpireDuration = Duration;
		NotificationInfo.FadeInDuration = 1.0f;
		NotificationInfo.FadeOutDuration = 1.0f;
		
		NotificationManager.Get().AddNotification(NotificationInfo);
	}

	inline EAppReturnType::Type Message(const EAppMsgType::Type Type, const FString& Title, const FString& Content)
	{
		return FMessageDialog::Open(Type, FText::FromString(Content), FText::FromString(Title));
	}

	inline void LogError(const FString& Content)
	{
		UE_LOG(LogTemp, Error, TEXT("[Auditor] %s"), *Content);
	}
}