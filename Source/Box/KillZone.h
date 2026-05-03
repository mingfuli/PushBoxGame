// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KillZone.generated.h"

class UBoxComponent;
class UPrimitiveComponent;
class USoundBase;

UCLASS()
class BOX_API AKillZone : public AActor
{
	GENERATED_BODY()

public:
	AKillZone();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = KillZone)
	bool bEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	USoundBase* DeathSound;

protected:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
