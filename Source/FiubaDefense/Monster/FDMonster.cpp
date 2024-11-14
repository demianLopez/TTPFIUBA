// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/FDMonster.h"

#include "Components/SphereComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Player/FDPlayerController.h"
#include "Player/FDPlayerState.h"
#include "Tower/FDTower.h"

// Sets default values
AFDMonster::AFDMonster()
{
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = SphereComponent;
	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnMovement"));
}

void AFDMonster::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	const FVector& Velocity = GetVelocity();
	SetActorRotation(Velocity.ToOrientationRotator());
}

float AFDMonster::TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	Health -= ActualDamage;

	if(Health <= 0.0f)
	{
		OnKilled(Cast<AFDPlayerController>(EventInstigator));
	}
	
	return ActualDamage;
}

void AFDMonster::OnKilled(AFDPlayerController* KilledByPlayer)
{
	AFDPlayerState* FDPlayerState = KilledByPlayer->GetPlayerState<AFDPlayerState>();
	FDPlayerState->IncrementKillCount();
	FDPlayerState->AddGold(GoldReward);
	
	Destroy();
}

bool AFDMonster::TryAttack(AFDTower* Tower)
{
	if(!IsValid(Tower))
		return false;

	FPointDamageEvent PointDamageEvent;
	PointDamageEvent.Damage = MonsterDamage;
		
	Tower->TakeDamage(MonsterDamage, PointDamageEvent, GetInstigatorController(), this);

	return true;
}
