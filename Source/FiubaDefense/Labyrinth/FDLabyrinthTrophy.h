// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Labyrinth/FDLabyrinthInteractiveObject.h"
#include "FDLabyrinthTrophy.generated.h"

/**
 * 
 */
UCLASS()
class FIUBADEFENSE_API AFDLabyrinthTrophy : public AFDLabyrinthInteractiveObject
{
	GENERATED_BODY()
	
public:
	AFDLabyrinthTrophy();
	
	virtual void OnOtherObjectTryToOverlap(AFDLabyrinthObject* Overlap, FDOVerlapResult& Result) override;
	virtual EFDLaberynthOverlapType CanOverlap(AFDLabyrinthObject* OtherObject) override;
};
