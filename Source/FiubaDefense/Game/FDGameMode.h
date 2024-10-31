// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FDGameMode.generated.h"

class AFDTower;
/**
 * 
 */
UCLASS()
class FIUBADEFENSE_API AFDGameMode : public AGameModeBase
{
	GENERATED_BODY()



public:
	virtual void SetPlayerDefaults(APawn* PlayerPawn) override;

protected:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AFDTower> PlayerTowerClass;
};
