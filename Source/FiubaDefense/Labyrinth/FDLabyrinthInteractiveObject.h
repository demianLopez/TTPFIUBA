// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Labyrinth/FDLabyrinthObject.h"
#include "FDLabyrinthInteractiveObject.generated.h"

UENUM()
enum class EFDLabyrinthInteractiveObjectType : uint8
{
	L_Player = 1,
	L_ExitDoor = 2,
	L_Enemy = 3,
	L_Trophy = 4,
	L_Trap = 5,
	L_Key = 6,
};



/**
 * 
 */
UCLASS()
class FIUBADEFENSE_API AFDLabyrinthInteractiveObject : public AFDLabyrinthObject
{
	GENERATED_BODY()

public:
	
	EFDLabyrinthInteractiveObjectType GetInteractiveObjectType() const { return InteractiveObjectType; }

protected:
	
	EFDLabyrinthInteractiveObjectType InteractiveObjectType;	
};
