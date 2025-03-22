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

}

AFDTower* AFDWeaponBase::GetOwningTower() const
{
	return GetOwner<AFDTower>();
}

void AFDWeaponBase::InitWithData(const UFDWeaponDataAsset* InWeaponDataAsset)
{
	WeaponData = InWeaponDataAsset;
	FireRate = WeaponData->BaseFireRate;
}

void AFDWeaponBase::TickAction()
{
	TurnsWithoutFiring++;

	if(TurnsWithoutFiring >= FireRate)
	{
		FindTarget();
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

	TurnsWithoutFiring = 0;
}

