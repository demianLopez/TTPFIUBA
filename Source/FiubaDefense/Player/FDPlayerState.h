// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FDPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class FIUBADEFENSE_API AFDPlayerState : public APlayerState
{
	GENERATED_BODY()
public:

	void IncrementIncome();

	FORCEINLINE float GetGold() const { return Gold; }
	FORCEINLINE int32 GetEnemiesKilled() const { return EnemiesKilled; }
	
protected:

	float Gold = 0.0f;
	int32 EnemiesKilled = 0;
	
};
