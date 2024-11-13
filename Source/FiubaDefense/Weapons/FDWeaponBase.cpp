// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/FDWeaponBase.h"
#include "Monster/FDMonster.h"
#include "Data/FDWeaponDataAsset.h"
#include "Monster/FDEnemySpawnerSubsystem.h"
#include "Projectiles/FDBaseProjectile.h"
#include "Tower/FDTower.h"

// Sets default values
AFDWeaponBase::AFDWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;
}

AFDTower* AFDWeaponBase::GetOwningTower() const
{
	return GetOwner<AFDTower>();
}

void AFDWeaponBase::InitWithData(const UFDWeaponDataAsset* InWeaponDataAsset)
{
	WeaponData = InWeaponDataAsset;
	FireRate = WeaponData->BaseFireRate;
	LastFireTimestamp = GetWorld()->GetTimeSeconds();
}

void AFDWeaponBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	float CurrentTimeStamp = GetWorld()->GetTimeSeconds();

	const float Delta = CurrentTimeStamp - LastFireTimestamp;

	if(Delta >= FireRate)
	{
		if(!IsValid(Target))
		{
			FindTarget();
		}

		FireProjectile();
	}
}

void AFDWeaponBase::FindTarget()
{
	UFDEnemySpawnerSubsystem* SpawnerSubsystem = UFDEnemySpawnerSubsystem::Get(this);

	const TArray<AFDMonster*>& SpawnedMonsters = SpawnerSubsystem->GetSpawnedMosnsters();

	int32 RandomIndex = FMath::RandRange(1, SpawnedMonsters.Num()) - 1;

	Target = SpawnedMonsters[RandomIndex];
}

void AFDWeaponBase::FireProjectile()
{
	if(!IsValid(Target))
		return;
	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AFDTower* OwningTower = GetOwningTower();
	
	AFDBaseProjectile* Projectile = GetWorld()->SpawnActor<AFDBaseProjectile>(ProjectileToSpawn
		, OwningTower->GetActorLocation(), OwningTower->GetActorRotation(), SpawnParameters);

	Projectile->GoToTarget(Target);

	LastFireTimestamp = GetWorld()->GetTimeSeconds();
}

