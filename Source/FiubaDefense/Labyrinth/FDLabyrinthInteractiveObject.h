// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Labyrinth/FDLabyrinthObject.h"
#include "FDLabyrinthInteractiveObject.generated.h"

UENUM()
enum class EFDLabyrinthInteractiveObjectType : uint8
{
	L_ExitDoor = 1,
	L_Trap = 2,
	L_Trophy = 3
};

/**
 * 
 */
UCLASS()
class FIUBADEFENSE_API AFDLabyrinthInteractiveObject : public AFDLabyrinthObject
{
	GENERATED_BODY()

public:

	virtual int32 GetIdentifier() const override;

	virtual void OnOtherObjectTryToOverlap(AFDLabyrinthObject* Overlap, FDOVerlapResult& Result) override;
	virtual bool CanOverlap(AFDLabyrinthObject* OtherObject) override;

protected:

	UPROPERTY(EditDefaultsOnly)
	EFDLabyrinthInteractiveObjectType InteractiveObjectType;
	
};
