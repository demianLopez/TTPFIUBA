// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FDBaseProjectile.generated.h"

class AFDPlayerController;
class UProjectileMovementComponent;
class UFDWeaponDataAsset;

UCLASS()
class FIUBADEFENSE_API AFDBaseProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFDBaseProjectile();

	void GoToTarget(AActor* Target);
	virtual void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY(Transient)
	TObjectPtr<AActor> Target;
		
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	AFDPlayerController* GetInstigatorController() const;

	const UFDWeaponDataAsset* FindWeaponData() const;

	virtual void OnImpact();

	float LastDistanceSQRT = 0.0f;
};
