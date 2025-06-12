// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Labyrinth/FDLabyrinthInteractiveObject.h"
#include "FDLabyrinthEnemy.generated.h"

/**
 * 
 */
UCLASS()
class FIUBADEFENSE_API AFDLabyrinthEnemy : public AFDLabyrinthInteractiveObject
{
	GENERATED_BODY()

public:

	AFDLabyrinthEnemy();

protected:
	virtual void BeginPlay() override;

public:
	virtual void OnOtherObjectTryToOverlap(AFDLabyrinthObject* Overlap, FDOVerlapResult& Result) override;
	virtual EFDLaberynthOverlapType CanOverlap(AFDLabyrinthObject* OtherObject) override;
};
