// Fill out your copyright notice in the Description page of Project Settings.


#include "Labyrinth/FDLabyrinthObject.h"

#include "FDLabyrinthGameMode.h"

void FDOVerlapResult::Clear()
{
	bDestroyObject = false;
	bEnemyCanMove = true;
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
		OutDirection.AddDefaulted(4);

		if (bApppendDistance)
		{
			OutDirection.Add(1.0f);
		}
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
	
	int32 Distance = FMath::Abs(DeltaX) + FMath::Abs(DeltaY) - 1;

	float NormalizedDistance = FMath::Clamp(static_cast<float>(Distance) / 2.0f, 0.0f, 1.0f);

	OutDirection.Add(NormalizedDistance);
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
