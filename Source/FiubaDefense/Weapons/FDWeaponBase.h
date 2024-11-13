// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FDWeaponBase.generated.h"

class AFDTower;
class UFDWeaponDataAsset;
class AFDBaseProjectile;

UCLASS()
class FIUBADEFENSE_API AFDWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFDWeaponBase();

	virtual void InitWithData(const UFDWeaponDataAsset* InWeaponDataAsset);

	virtual void Tick(float DeltaSeconds) override;

	AFDTower* GetOwningTower() const;
protected:

	UPROPERTY(Transient)
	TObjectPtr<const UFDWeaponDataAsset> WeaponData;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AFDBaseProjectile> ProjectileToSpawn;

	float FireRate = 1.0f;
	float LastFireTimestamp = 0.0f;

	UPROPERTY(Transient)
	TObjectPtr<AActor> Target;

	void FindTarget();
	void FireProjectile();
};
