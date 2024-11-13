// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectiles/FDBaseProjectile.h"
#include "FDSingleTargetProjectile.generated.h"

/**
 * 
 */
UCLASS()
class FIUBADEFENSE_API AFDSingleTargetProjectile : public AFDBaseProjectile
{
	GENERATED_BODY()

protected:
	virtual void OnImpact() override;
};
