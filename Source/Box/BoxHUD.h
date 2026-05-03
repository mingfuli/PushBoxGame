// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BoxHUD.generated.h"

class UBoxHUDWidget;

UCLASS()
class BOX_API ABoxHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<UBoxHUDWidget> HUDWidgetClass;

	UFUNCTION(BlueprintCallable, Category = "HUD")
	UBoxHUDWidget* GetHUDWidget() const;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	TObjectPtr<UBoxHUDWidget> HUDWidget;
};
