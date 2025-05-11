// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectiles/FDSingleTargetProjectile.h"

#include "Data/FDWeaponDataAsset.h"
#include "Engine/DamageEvents.h"
#include "Player/FDPlayerController.h"

void AFDSingleTargetProjectile::OnImpact()
{
	const UFDWeaponDataAsset* WeaponDataAsset = FindWeaponData();
	if(IsValid(Target) && IsValid(WeaponDataAsset))
	{
		float Damage = WeaponDataAsset->Damage;
		FPointDamageEvent PointDamageEvent;
		PointDamageEvent.Damage = Damage;
		
		Target->TakeDamage(Damage, PointDamageEvent, GetInstigatorController(), this);
	}
	
	Super::OnImpact();
}
