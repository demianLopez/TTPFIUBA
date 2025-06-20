// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FDLabyrinthInteractiveObject.h"
#include "FDLabyrinthPlayer.generated.h"

UCLASS()
class FIUBADEFENSE_API AFDLabyrinthPlayer : public AFDLabyrinthInteractiveObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFDLabyrinthPlayer();
	
	virtual void OnOtherObjectTryToOverlap(AFDLabyrinthObject* Overlap, FDOVerlapResult& Result) override;
	virtual EFDLaberynthOverlapType CanOverlap(AFDLabyrinthObject* OtherObject) override;
};
