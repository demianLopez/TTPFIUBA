// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "FiubaDefenseDeveloperSettings.generated.h"

/**
 * 
 */
UCLASS(config=Game, defaultconfig, meta = (DisplayName = "Fiuba Defense"))
class FIUBADEFENSE_API UFiubaDefenseDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	static const UFiubaDefenseDeveloperSettings* Get();

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category="Spawn")
	float BaseSpawn = 0.5f;

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category="Spawn")
	float SpawnGrowthFactor = 0.5f;

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category="Spawn")
	float TurnsConstant = 20.0f;

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category="Spawn")
	float SpawnMinDistance = 1000.0f;

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category="Spawn")
	float SpawnMaxDistance = 1000.0f;
};
