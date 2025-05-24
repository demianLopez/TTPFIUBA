// Fill out your copyright notice in the Description page of Project Settings.


#include "Labyrinth/FDLabyrinthObject.h"

#include "FDLabyrinthGameMode.h"

AFDLabyrinthObject::AFDLabyrinthObject()
{
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
}

void AFDLabyrinthObject::SetGridLocation(int32 InLocationX, int32 InLocationY)
{
	LocationX = InLocationX;
	LocationY = InLocationY;
}

void AFDLabyrinthObject::GetGridLocation(int32& OutLocationX, int32& OutLocationY)
{
	OutLocationX = LocationX;
	OutLocationY = LocationY;
}

void AFDLabyrinthObject::Destroyed()
{
	UWorld* World = GetWorld();

	if (IsValid(World) && World->IsGameWorld())
	{
		AFDLabyrinthGameMode* GameMode = GetWorld()->GetAuthGameMode<AFDLabyrinthGameMode>();
		GameMode->ObjectRemoved(this);
	}
	
	Super::Destroyed();
}
