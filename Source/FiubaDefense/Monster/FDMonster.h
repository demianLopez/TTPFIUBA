// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "FDMonster.generated.h"

class AFDTower;
class AFDPlayerController;
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

	float GetAttackRange() const { return AttackRange; }

	bool TryAttack(AFDTower* Tower);
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UFloatingPawnMovement> FloatingPawnMovement;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USphereComponent> SphereComponent;
	
	float Health = 2.0f;
	float GoldReward = 5.0f;
	float AttackRange = 200.0f;
	float MonsterDamage = 5.0f;

	void OnKilled(AFDPlayerController* KilledByPlayer);
};
