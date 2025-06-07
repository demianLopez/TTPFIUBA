// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Labyrinth/FDLabyrinthInteractiveObject.h"
#include "FDLabyrinthTrap.generated.h"

/**
 * 
 */
UCLASS()
class FIUBADEFENSE_API AFDLabyrinthTrap : public AFDLabyrinthInteractiveObject
{
	GENERATED_BODY()

public:

	AFDLabyrinthTrap();

	virtual EFDLaberynthOverlapType CanOverlap(AFDLabyrinthObject* OtherObject) override;
	virtual void OnOtherObjectTryToOverlap(AFDLabyrinthObject* Overlap, FDOVerlapResult& Result) override;
};
