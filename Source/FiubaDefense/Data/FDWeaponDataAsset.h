// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FDDamageTypes.h"
#include "Engine/DataAsset.h"
#include "FDWeaponDataAsset.generated.h"

class AFDWeaponBase;
/**
 * 
 */
UCLASS()
class FIUBADEFENSE_API UFDWeaponDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, meta = (AssetBundles="Game"))
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly)
	FText WeaponName;

	UPROPERTY(EditDefaultsOnly)
	FText WeaponDescription;

	UPROPERTY(EditDefaultsOnly)
	float BasePrice = 1000.0f;

	UPROPERTY(EditDefaultsOnly)
	EFDWeaponDamageType DamageType;

	UPROPERTY(EditDefaultsOnly, meta = (AssetBundles="Game"))
	TSoftClassPtr<AFDWeaponBase> WeaponClass;

	UPROPERTY(EditDefaultsOnly)
	int32 BaseFireRate = 1.0f;
};
