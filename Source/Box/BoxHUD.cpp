// Copyright Epic Games, Inc. All Rights Reserved.

#include "BoxHUD.h"
#include "BoxHUDWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void ABoxHUD::BeginPlay()
{
	Super::BeginPlay();

	if (!HUDWidgetClass)
	{
		return;
	}

	HUDWidget = CreateWidget<UBoxHUDWidget>(GetWorld(), HUDWidgetClass);
	if (!HUDWidget)
	{
		return;
	}

	bool bIsLevel1 = false;
	if (UWorld* World = GetWorld())
	{
		const FString LevelName = UGameplayStatics::GetCurrentLevelName(World);
		bIsLevel1 = LevelName.Contains(TEXT("Level1"));
	}

	HUDWidget->Setup(bIsLevel1);
	HUDWidget->AddToViewport();
}

UBoxHUDWidget* ABoxHUD::GetHUDWidget() const
{
	return HUDWidget;
}
