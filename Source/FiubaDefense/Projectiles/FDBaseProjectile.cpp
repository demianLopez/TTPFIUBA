// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectiles/FDBaseProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"

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
}


void AFDBaseProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	float ImpactDistanceSQRT = FMath::Square(ImpactDistance);
	
	if(IsValid(Target))
	{
		float DistSQRToTarget = GetSquaredDistanceTo(Target);

		if(DistSQRToTarget < ImpactDistanceSQRT)
		{
			OnImpact();
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

