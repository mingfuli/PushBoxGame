// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BoxHUDWidget.generated.h"

class UTextBlock;

UCLASS()
class BOX_API UBoxHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD|InputHint", meta = (MultiLine = true))
	FText InputHintText = NSLOCTEXT("BoxHUD", "InputHint", "W/A/S/D 移动\nSpace 跳跃\nR 重开");

	UPROPERTY(BlueprintReadWrite, Category = "HUD", meta = (BindWidget))
	TObjectPtr<UTextBlock> InputHintTextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD|Victory")
	FString VictoryText = TEXT("Victory!");

	UPROPERTY(BlueprintReadWrite, Category = "HUD", meta = (BindWidget))
	TObjectPtr<UTextBlock> VictoryTextBlock;

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void Setup(bool bIsLevel1);

	UFUNCTION(BlueprintCallable, Category = "HUD|Victory")
	void ShowVictoryText();

	UFUNCTION(BlueprintCallable, Category = "HUD|Victory")
	void HideVictoryText();

	UFUNCTION(BlueprintPure, Category = "HUD|Victory")
	bool IsVictoryTextVisible() const { return bShowVictoryText; }

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD|Victory")
	void OnVictoryTextVisibilityChanged(bool bIsVisible);

protected:
	bool bShowVictoryText = false;
};
