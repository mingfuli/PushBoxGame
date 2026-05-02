// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyBox.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

AMyBox::AMyBox()
	: CurrentState(EBoxState::Idle)
	, MoveElapsed(0.0f)
	, StartLocation(FVector::ZeroVector)
	, TargetLocation(FVector::ZeroVector)
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;
	CollisionBox->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMyBox::BeginPlay()
{
	Super::BeginPlay();
}

void AMyBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState == EBoxState::Moving)
	{
		MoveElapsed += DeltaTime;
		float Alpha = FMath::Clamp(MoveElapsed / MoveDuration, 0.0f, 1.0f);
		SetActorLocation(FMath::Lerp(StartLocation, TargetLocation, Alpha));

		if (MoveElapsed >= MoveDuration)
		{
			SetActorLocation(TargetLocation);
			DetachPlayer();
			CurrentState = EBoxState::Idle;
			OnBoxMoveFinished.Broadcast();
		}
	}
}

bool AMyBox::CanMoveTo(FVector Target) const
{
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	if (OverlappingPlayer.IsValid())
	{
		Params.AddIgnoredActor(OverlappingPlayer.Get());
	}

	FVector Extent = CollisionBox->GetScaledBoxExtent();
	FCollisionShape Shape = FCollisionShape::MakeBox(Extent * 0.9f);

	FVector Start = GetActorLocation();
	return !GetWorld()->SweepSingleByChannel(Hit, Start, Target, FQuat::Identity, ECC_Visibility, Shape, Params);
}

void AMyBox::BeginMove(FVector Direction, ACharacter* Player)
{
	AttachPlayer(Player);
	StartLocation = GetActorLocation();
	TargetLocation = StartLocation + Direction * MoveDistance;
	MoveElapsed = 0.0f;
	CurrentState = EBoxState::Moving;
}

void AMyBox::AttachPlayer(ACharacter* Player)
{
	if (!Player)
	{
		return;
	}

	OverlappingPlayer = Player;

	UCharacterMovementComponent* MoveComp = Player->GetCharacterMovement();
	if (MoveComp)
	{
		MoveComp->StopMovementImmediately();
		MoveComp->SetMovementMode(MOVE_None);
	}

	Player->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

	if (PushMontage && Player->GetMesh() && Player->GetMesh()->GetAnimInstance())
	{
		Player->PlayAnimMontage(PushMontage);
	}
}

void AMyBox::DetachPlayer()
{
	ACharacter* Player = OverlappingPlayer.Get();
	if (!Player)
	{
		return;
	}

	Player->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	UCharacterMovementComponent* MoveComp = Player->GetCharacterMovement();
	if (MoveComp)
	{
		MoveComp->SetMovementMode(MOVE_Walking);
	}

	OverlappingPlayer.Reset();
}
