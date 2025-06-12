// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FDLabyrinthObject.generated.h"

struct FDOVerlapResult
{
	bool bGameEnd = false;
	bool bWonGame = false;
	bool bPlayerKilledByTrap = false;
	bool bPlayerKilledByEnemy = false;
	bool bGrabbedTrophy = false;
	bool bDestroyObject = false;
	bool bGrabbedKey = false;
	bool bEnemyCanMove = true;

	void Clear();
};

UENUM()
enum class EFDLaberynthOverlapType : uint8
{
	Blocked,
	Overlapped,
	Ignored,
};

UCLASS()
class FIUBADEFENSE_API AFDLabyrinthObject : public AActor
{
	GENERATED_BODY()

public:

	AFDLabyrinthObject();

	void SetGridLocation(int32 InLocationX, int32 InLocationY);
	void GetGridLocation(int32& OutLocationX, int32& OutLocationY);
	
	virtual void OnOtherObjectTryToOverlap(AFDLabyrinthObject* Overlap, FDOVerlapResult& Result) { }
	virtual EFDLaberynthOverlapType CanOverlap(AFDLabyrinthObject* OtherObject) { return EFDLaberynthOverlapType::Ignored; }
	
	void AppendDirectionToObject(AFDLabyrinthObject* OtherObject, TArray<float>& OutDirection, bool bApppendDistance = true) const;

	void MarkAsDestroyed();
	void OnCreated();
	
protected:
	int32 LocationX;
	int32 LocationY;

	bool bIsDestroyed;
};
