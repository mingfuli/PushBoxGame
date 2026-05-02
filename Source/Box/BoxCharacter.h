// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "BoxCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class AMyBox;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UENUM()
enum class EPushState : uint8
{
	None,
	ReadyToPush,
	Pushing
};

UCLASS(config=Game)
class ABoxCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	// USpringArmComponent* CameraBoom;

	/** Follow camera */
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	// UCameraComponent* FollowCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	// UInputAction* LookAction;

	/** Restart Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RestartAction;

	AMyBox* OverlappingBox;

	UPROPERTY(Transient)
	EPushState PushState;

public:
	ABoxCharacter();

	UFUNCTION(BlueprintPure, Category = "Push")
	bool IsReadyToPush() const { return PushState == EPushState::ReadyToPush; }

	UFUNCTION(BlueprintPure, Category = "Push")
	bool IsPushing() const { return PushState == EPushState::Pushing; }

protected:
	void Move(const FInputActionValue& Value);

	void RestartLevel();

	void HandlePushInput(FVector2D Input);
	void ResetPush();
	void SnapToPushPosition(FVector Direction, AMyBox* Box);
	FVector GetPushDirection(AMyBox* Box, FVector Input) const;

	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual void NotifyControllerChanged() override;

	virtual bool CanJumpInternal_Implementation() const override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnBoxMoveFinishedHandler();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
