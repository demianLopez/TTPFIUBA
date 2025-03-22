// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower/FDTower.h"

#include "FDTowerGridComponent.h"
#include "Components/CapsuleComponent.h"
#include "Player/FDPlayerController.h"
#include "Weapons/FDWeaponBase.h"

// Sets default values
AFDTower::AFDTower()
{
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("RootCapsule");
	TowerGridComponent = CreateDefaultSubobject<UFDTowerGridComponent>("FDTowerGrid");
	
	SetRootComponent(CapsuleComponent);
	TowerGridComponent->SetupAttachment(RootComponent);
}

float AFDTower::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	CurrentHealth = FMath::Clamp(CurrentHealth - ActualDamage, 0.0f, MaxHealth);

	if(CurrentHealth <= 0.0f)
	{
		OnTowerKilled();
	}
	
	return ActualDamage;
}

AFDPlayerController* AFDTower::GetOwningPlayer() const
{
	return GetOwner<AFDPlayerController>();
}

FVector AFDTower::GetFeetLocation() const
{
	return GetActorLocation() - GetFeetLocationOffset();
}

FVector AFDTower::GetFeetLocationOffset() const
{
	const float CapsuleHalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();
	return FVector(0.0f, 0.0f, CapsuleHalfHeight);
}

void AFDTower::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	MaxHealth = InitialHealth;
	CurrentHealth = MaxHealth;
}

void AFDTower::AddWeapon(AFDWeaponBase* Weapon)
{
	Weapons.Add(Weapon);
}

void AFDTower::TickTower()
{
	for (TWeakObjectPtr<AFDWeaponBase> Weapon : Weapons)
	{
		Weapon->TickAction();
	}
}

void AFDTower::OnTowerKilled()
{
}
