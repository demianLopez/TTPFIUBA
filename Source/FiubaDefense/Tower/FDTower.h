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

	virtual void PostInitializeComponents() override;

	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	UPROPERTY(EditDefaultsOnly)
	float InitialHealth = 100.0f;

	float CurrentHealth;
	float MaxHealth;
};
