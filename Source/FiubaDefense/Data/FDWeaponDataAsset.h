// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FDDamageTypes.h"
#include "Engine/DataAsset.h"
#include "FDWeaponDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class FIUBADEFENSE_API UFDWeaponDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UTexture> Icon;

	UPROPERTY(EditDefaultsOnly)
	EFDWeaponDamageType DamageType;
};
