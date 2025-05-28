// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FDLabyrinthObject.generated.h"

struct FDOVerlapResult
{
	bool bGameEnd = false;
	bool bWonGame = false;
	bool bGrabbedTrophy = false;
	bool bDestroyObject = false;
};

UCLASS()
class FIUBADEFENSE_API AFDLabyrinthObject : public AActor
{
	GENERATED_BODY()

public:

	AFDLabyrinthObject();

	void SetGridLocation(int32 InLocationX, int32 InLocationY);
	void GetGridLocation(int32& OutLocationX, int32& OutLocationY);

	virtual int32 GetIdentifier() const { return -1; }

	virtual void OnOtherObjectTryToOverlap(AFDLabyrinthObject* Overlap, FDOVerlapResult& Result) { }
	virtual bool CanOverlap(AFDLabyrinthObject* OtherObject) { return false; }
	virtual void Destroyed() override;
protected:
	int32 LocationX;
	int32 LocationY;
};
