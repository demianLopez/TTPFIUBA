// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower/FDTower.h"

#include "Components/CapsuleComponent.h"

// Sets default values
AFDTower::AFDTower()
{
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("RootCapsule");
	SetRootComponent(CapsuleComponent);
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
