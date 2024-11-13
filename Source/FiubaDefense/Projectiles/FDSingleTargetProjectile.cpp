// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectiles/FDSingleTargetProjectile.h"

#include "Engine/DamageEvents.h"

void AFDSingleTargetProjectile::OnImpact()
{
	if(IsValid(Target))
	{
		FPointDamageEvent PointDamageEvent;
		PointDamageEvent.Damage = 1.0f;
		
		Target->TakeDamage(1.0f, PointDamageEvent, nullptr, this);
	}
	
	Super::OnImpact();
}
