// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FDTower.generated.h"

class UCapsuleComponent;

UCLASS()
class FIUBADEFENSE_API AFDTower : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFDTower();

	FVector GetFeetLocationOffset() const;

protected:

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

};
