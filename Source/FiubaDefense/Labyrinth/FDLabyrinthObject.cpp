// Fill out your copyright notice in the Description page of Project Settings.


#include "Labyrinth/FDLabyrinthObject.h"

#include "FDLabyrinthGameMode.h"

void FDOVerlapResult::Clear()
{
	bDestroyObject = false;
}

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

void AFDLabyrinthObject::AppendDirectionToObject(AFDLabyrinthObject* OtherObject, TArray<float>& OutDirection, bool bApppendDistance) const
{
	if (!IsValid(OtherObject) || OtherObject->bIsDestroyed)
	{
		int32 AppendSize = bApppendDistance ? 6 : 4;
		OutDirection.AddDefaulted(AppendSize);
		return;
	}
	
	int32 ToX, ToY;
	OtherObject->GetGridLocation(ToX, ToY);
	
	int32 DeltaX = ToX - LocationX;
	int32 DeltaY = ToY - LocationY;

	OutDirection.Add(DeltaX > 0 ? 1 : 0);
	OutDirection.Add(DeltaY > 0 ? 1 : 0);
	OutDirection.Add(DeltaX < 0 ? 1 : 0);
	OutDirection.Add(DeltaY < 0 ? 1 : 0);

	if (!bApppendDistance)
		return;
	
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

void AFDLabyrinthObject::MarkAsDestroyed()
{
	bIsDestroyed = true;
	SetActorHiddenInGame(true);
}

void AFDLabyrinthObject::OnCreated()
{
	SetActorHiddenInGame(false);
	bIsDestroyed = false;
}
