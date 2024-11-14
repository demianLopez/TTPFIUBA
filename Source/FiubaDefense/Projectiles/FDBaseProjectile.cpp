// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectiles/FDBaseProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Tower/FDTower.h"
#include "Weapons/FDWeaponBase.h"

// Sets default values
AFDBaseProjectile::AFDBaseProjectile()
{
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.2;
}

void AFDBaseProjectile::GoToTarget(AActor* InTarget)
{
	ProjectileMovementComponent->bIsHomingProjectile = true;
	ProjectileMovementComponent->HomingTargetComponent = InTarget->GetRootComponent();

	Target = InTarget;
	LastDistanceSQRT = GetSquaredDistanceTo(Target);
}

AFDPlayerController* AFDBaseProjectile::GetInstigatorController() const
{
	AFDWeaponBase* OwningWeapon = GetOwner<AFDWeaponBase>();

	if(!IsValid(OwningWeapon))
		return nullptr;

	AFDTower* Tower = OwningWeapon->GetOwningTower();

	if(!IsValid(Tower))
		return nullptr;

	return Tower->GetOwningPlayer();
}

void AFDBaseProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
		
	if(IsValid(Target))
	{
		float DistSQRToTarget = GetSquaredDistanceTo(Target);

		if(LastDistanceSQRT < DistSQRToTarget)
		{
			OnImpact();
		}
		else
		{
			LastDistanceSQRT = GetSquaredDistanceTo(Target);
		}
	}
	else
	{
		Destroy();
	}
}

void AFDBaseProjectile::OnImpact()
{
	Destroy();
}

