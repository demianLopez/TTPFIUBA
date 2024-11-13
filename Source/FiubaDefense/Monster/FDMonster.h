// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "FDMonster.generated.h"

class USphereComponent;
class UFloatingPawnMovement;

UCLASS()
class FIUBADEFENSE_API AFDMonster : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFDMonster();

	virtual void TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
	virtual float TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UFloatingPawnMovement> FloatingPawnMovement;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USphereComponent> SphereComponent;

	float Health = 2.0f;
};
