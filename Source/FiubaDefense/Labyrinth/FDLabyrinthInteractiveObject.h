// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Labyrinth/FDLabyrinthObject.h"
#include "FDLabyrinthInteractiveObject.generated.h"

UENUM()
enum class EFDLabyrinthInteractiveObjectType : uint8
{
	L_ExitDoor = 2,
	L_Trap = 3,
	L_Trophy = 4
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

	EFDLabyrinthInteractiveObjectType GetInteractiveObjectType() const { return InteractiveObjectType; }
protected:

	UPROPERTY(EditDefaultsOnly)
	EFDLabyrinthInteractiveObjectType InteractiveObjectType;
	
};
