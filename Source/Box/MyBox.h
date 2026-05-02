// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyBox.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class ACharacter;

UENUM()
enum class EBoxState : uint8
{
	Idle,
	Moving
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBoxMoveFinished);

UCLASS()
class BOX_API AMyBox : public AActor
{
	GENERATED_BODY()

public:
	AMyBox();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float MoveDistance = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float MoveDuration = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	class UAnimMontage* PushMontage;

	bool CanMoveTo(FVector Target) const;
	void BeginMove(FVector Direction, ACharacter* Player);
	void AttachPlayer(ACharacter* Player);
	void DetachPlayer();

	bool IsMoving() const { return CurrentState == EBoxState::Moving; }

	UPROPERTY(BlueprintAssignable)
	FOnBoxMoveFinished OnBoxMoveFinished;

private:
	EBoxState CurrentState;
	TWeakObjectPtr<ACharacter> OverlappingPlayer;
	FVector StartLocation;
	FVector TargetLocation;
	float MoveElapsed;
};
