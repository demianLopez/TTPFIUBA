// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Labyrinth/FDLabyrinthInteractiveObject.h"
#include "FDLabyrinthDoor.generated.h"

/**
 * 
 */
UCLASS()
class FIUBADEFENSE_API AFDLabyrinthDoor : public AFDLabyrinthInteractiveObject
{
	GENERATED_BODY()

public:

	AFDLabyrinthDoor();

	virtual void OnOtherObjectTryToOverlap(AFDLabyrinthObject* Overlap, FDOVerlapResult& Result) override;
	virtual EFDLaberynthOverlapType CanOverlap(AFDLabyrinthObject* OtherObject) override;
};
