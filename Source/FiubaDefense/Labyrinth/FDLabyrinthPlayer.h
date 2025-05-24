// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Labyrinth/FDLabyrinthObject.h"
#include "FDLabyrinthPlayer.generated.h"

UCLASS()
class FIUBADEFENSE_API AFDLabyrinthPlayer : public AFDLabyrinthObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFDLabyrinthPlayer();
	
	virtual int32 GetIdentifier() const override { return 0; }
};
