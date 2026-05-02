// Copyright Epic Games, Inc. All Rights Reserved.

#include "BoxGameMode.h"
#include "BoxCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABoxGameMode::ABoxGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
