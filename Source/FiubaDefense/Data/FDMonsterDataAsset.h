// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FDDamageTypes.h"
#include "FDMonsterDataAsset.generated.h"

class AFDMonster;
/**
 * 
 */
UCLASS()
class FIUBADEFENSE_API UFDMonsterDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<AFDMonster> MonsterToSpawnClass;

	UPROPERTY(EditDefaultsOnly)
	EFDMonsterArmorType ArmorType;

	UPROPERTY(EditDefaultsOnly)
	float MinHealth = 10.0f;

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 100.0f;
};
