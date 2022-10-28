// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestServerClientGameMode.h"
#include "TestServerClientCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATestServerClientGameMode::ATestServerClientGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
