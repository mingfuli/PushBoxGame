// Copyright Epic Games, Inc. All Rights Reserved.

#include "BoxCharacter.h"
#include "MyBox.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

ABoxCharacter::ABoxCharacter()
	: OverlappingBox(nullptr)
	, PushState(EPushState::None)
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 350.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
}

void ABoxCharacter::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (PushState != EPushState::None)
	{
		return;
	}

	if (AMyBox* Box = Cast<AMyBox>(Other))
	{
		if (GetCharacterMovement()->IsFalling())
		{
			return;
		}

		if (HitNormal.Z > 0.5f)
		{
			return;
		}

		FVector PlayerToBox = Box->GetActorLocation() - GetActorLocation();
		PlayerToBox.Z = 0.0f;
		FVector SnapDir;
		if (FMath::Abs(PlayerToBox.X) > FMath::Abs(PlayerToBox.Y))
		{
			SnapDir = FVector(FMath::Sign(PlayerToBox.X), 0.0f, 0.0f);
		}
		else
		{
			SnapDir = FVector(0.0f, FMath::Sign(PlayerToBox.Y), 0.0f);
		}
		SnapToPushPosition(SnapDir, Box);

		OverlappingBox = Box;
		PushState = EPushState::ReadyToPush;
		
		Box->OnBoxMoveFinished.AddDynamic(this, &ABoxCharacter::OnBoxMoveFinishedHandler);
	}
}

void ABoxCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

bool ABoxCharacter::CanJumpInternal_Implementation() const
{
	if (PushState != EPushState::None)
	{
		return false;
	}

	return Super::CanJumpInternal_Implementation();
}

void ABoxCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const TCHAR* StateText = TEXT("None");
	switch (PushState)
	{
	case EPushState::ReadyToPush:
		StateText = TEXT("ReadyToPush");
		break;
	case EPushState::Pushing:
		StateText = TEXT("Pushing");
		break;
	default:
		break;
	}

	//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.0f, FColor::Cyan, FString::Printf(TEXT("PushState: %s"), StateText));
}

void ABoxCharacter::OnBoxMoveFinishedHandler()
{
	PushState = EPushState::ReadyToPush;

	if (USkeletalMeshComponent* SkelMesh = GetMesh())
	{
		if (UAnimInstance* AnimInst = SkelMesh->GetAnimInstance())
		{
			AnimInst->SavePoseSnapshot(TEXT("PushPose"));
		}
	}
}

void ABoxCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABoxCharacter::Move);

		EnhancedInputComponent->BindAction(RestartAction, ETriggerEvent::Started, this, &ABoxCharacter::RestartLevel);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ABoxCharacter::Move(const FInputActionValue& Value)
{
	if (PushState == EPushState::Pushing)
	{
		return;
	}

	if (PushState == EPushState::ReadyToPush)
	{
		HandlePushInput(Value.Get<FVector2D>());
		return;
	}

	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ABoxCharacter::RestartLevel()
{
	UWorld* World = GetWorld();
	if (World)
	{
		FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(World);
		UGameplayStatics::OpenLevel(World, FName(*CurrentLevelName));
	}
}

void ABoxCharacter::HandlePushInput(FVector2D Input)
{
	if (!OverlappingBox)
	{
		ResetPush();
		return;
	}

	FVector WorldInput = FVector::ZeroVector;
	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		WorldInput = ForwardDirection * Input.Y + RightDirection * Input.X;
	}

	FVector Direction = GetPushDirection(OverlappingBox, WorldInput);
	if (Direction.IsNearlyZero())
	{
		ResetPush();
		return;
	}

	FVector Target = OverlappingBox->GetActorLocation() + Direction * OverlappingBox->MoveDistance;
	if (OverlappingBox->CanMoveTo(Target))
	{
		OverlappingBox->AttachPlayer(this);
		OverlappingBox->BeginMove(Direction, this);
		PushState = EPushState::Pushing;
	}
	// else
	// {
	// 	ResetPush();
	// }
}

void ABoxCharacter::ResetPush()
{
	if (OverlappingBox)
	{
		OverlappingBox->OnBoxMoveFinished.RemoveDynamic(this, &ABoxCharacter::OnBoxMoveFinishedHandler);
		OverlappingBox->DetachPlayer();
		OverlappingBox = nullptr;
	}
	PushState = EPushState::None;
}

void ABoxCharacter::SnapToPushPosition(FVector Direction, AMyBox* Box)
{
	if (PushState == EPushState::None)
	{
		AddActorWorldOffset(-Direction * SnapDistance);
		FVector ToBox = Box->GetActorLocation() - GetActorLocation();
		ToBox.Z = 0.0f;
		SetActorRotation(ToBox.Rotation());
	}
}

FVector ABoxCharacter::GetPushDirection(AMyBox* Box, FVector Input) const
{
	if (Input.IsNearlyZero())
	{
		return FVector::ZeroVector;
	}

	float AbsX = FMath::Abs(Input.X);
	float AbsY = FMath::Abs(Input.Y);

	FVector Direction;
	if (AbsX > AbsY)
	{
		Direction = FVector(FMath::Sign(Input.X), 0.0f, 0.0f);
	}
	else
	{
		Direction = FVector(0.0f, FMath::Sign(Input.Y), 0.0f);
	}

	if (Direction.IsNearlyZero())
	{
		return FVector::ZeroVector;
	}

	FVector PlayerToBox = Box->GetActorLocation() - GetActorLocation();
	PlayerToBox.Z = 0.0f;

	float DotForward = FVector::DotProduct(PlayerToBox, Direction);
	if (DotForward <= 0.0f)
	{
		return FVector::ZeroVector;
	}

	FVector SideDir = FVector(-Direction.Y, Direction.X, 0.0f);
	float DotSide = FMath::Abs(FVector::DotProduct(PlayerToBox, SideDir));

	if (DotSide > DotForward)
	{
		return FVector::ZeroVector;
	}

	return Direction;
}
