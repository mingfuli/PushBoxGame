// Copyright Epic Games, Inc. All Rights Reserved.

#include "KillZone.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Sound/SoundBase.h"

AKillZone::AKillZone()
{
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;
	CollisionBox->SetBoxExtent(FVector(500.0f, 500.0f, 50.0f));
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AKillZone::OnOverlapBegin);
}

void AKillZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bEnabled)
	{
		return;
	}

	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	if (OtherActor->IsA(ACharacter::StaticClass()))
	{
		UWorld* World = GetWorld();
		if (!World)
		{
			return;
		}

		float Delay = 0.0f;
		if (DeathSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, DeathSound, OtherActor->GetActorLocation());
			Delay = DeathSound->GetDuration();
		}

		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(TimerHandle, [World]()
		{
			FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(World);
			UGameplayStatics::OpenLevel(World, FName(*CurrentLevelName));
		}, Delay, false);
	}
}
