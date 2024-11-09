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
	float K1 = 5.0f;

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category="Spawn")
	float K2 = 0.01f;

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category="Spawn")
	float K3 = 0.05f;

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category="Spawn")
	float SpawnMinDistance = 1000.0f;

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category="Spawn")
	float SpawnMaxDistance = 1000.0f;
};
