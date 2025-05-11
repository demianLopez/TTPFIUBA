// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "FiubaDefenseAISettings.generated.h"

/**
 * 
 */
UCLASS(config=Game, defaultconfig, meta = (DisplayName = "Fiuba AI Settings"))
class FIUBADEFENSE_API UFiubaDefenseAISettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	static const UFiubaDefenseAISettings* Get();

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category="Spawn")
	float DelayBetweenRounds = 0.0f;
};
