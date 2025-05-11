// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FDTower.generated.h"

class UFDTowerGridComponent;
class UCapsuleComponent;
class AFDPlayerController;
class AFDWeaponBase;

UCLASS()
class FIUBADEFENSE_API AFDTower : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFDTower();

	FVector GetFeetLocationOffset() const;
	FVector GetFeetLocation() const;

	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator,
		AActor* DamageCauser) override;

	virtual void PostInitializeComponents() override;

	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }

	AFDPlayerController* GetOwningPlayer() const;
	void AddWeapon(AFDWeaponBase* Weapon);

	void TickTower();

	UFDTowerGridComponent* GetGridComponent() const { return TowerGridComponent; }

	int32 GetWeaponNumbers() const { return Weapons.Num(); }
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UFDTowerGridComponent> TowerGridComponent;

	UPROPERTY(EditDefaultsOnly)
	float InitialHealth = 100.0f;

	float CurrentHealth;
	float MaxHealth;

	TArray<TWeakObjectPtr<AFDWeaponBase>> Weapons;
	
	void OnTowerKilled();
};
