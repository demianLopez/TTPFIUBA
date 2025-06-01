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

void AFDLabyrinthObject::AppendDirectionToObject(AFDLabyrinthObject* OtherObject, TArray<float>& OutDirection) const
{
	int32 FromX, FromY;
	OtherObject->GetGridLocation(FromX, FromY);
	
	int32 DeltaX = LocationX - FromX;
	int32 DeltaY = LocationY - FromY;

	OutDirection.Add(DeltaX > 0 ? 1 : 0);
	OutDirection.Add(DeltaY > 0 ? 1 : 0);
	OutDirection.Add(DeltaX < 0 ? 1 : 0);
	OutDirection.Add(DeltaY < 0 ? 1 : 0);

	int32 Distance = FMath::Abs(DeltaX) + FMath::Abs(DeltaY);

	if (Distance == 0)
	{
		OutDirection.Add(0); OutDirection.Add(0);
	} else if (Distance == 1)
	{
		OutDirection.Add(0); OutDirection.Add(1);
	} else if (Distance == 2)
	{
		OutDirection.Add(1); OutDirection.Add(0);
	} else
	{
		OutDirection.Add(1); OutDirection.Add(1);
	}	
}
