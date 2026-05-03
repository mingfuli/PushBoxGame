// Copyright Epic Games, Inc. All Rights Reserved.

#include "BoxHUDWidget.h"
#include "Components/TextBlock.h"

void UBoxHUDWidget::Setup(bool bIsLevel1)
{
	if (InputHintTextBlock)
	{
		InputHintTextBlock->SetText(InputHintText);
		InputHintTextBlock->SetVisibility(bIsLevel1 ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	if (VictoryTextBlock)
	{
		VictoryTextBlock->SetText(FText::FromString(VictoryText));
		VictoryTextBlock->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UBoxHUDWidget::ShowVictoryText()
{
	if (!bShowVictoryText)
	{
		bShowVictoryText = true;
		if (VictoryTextBlock)
		{
			VictoryTextBlock->SetVisibility(ESlateVisibility::Visible);
		}
		OnVictoryTextVisibilityChanged(true);
	}
}

void UBoxHUDWidget::HideVictoryText()
{
	if (bShowVictoryText)
	{
		bShowVictoryText = false;
		if (VictoryTextBlock)
		{
			VictoryTextBlock->SetVisibility(ESlateVisibility::Collapsed);
		}
		OnVictoryTextVisibilityChanged(false);
	}
}
