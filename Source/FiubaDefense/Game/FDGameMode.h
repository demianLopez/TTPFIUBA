// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FDGameMode.generated.h"

class UFDWeaponDataAsset;
class AFDWeaponBase;
class AFDTower;
/**
 * 
 */
UCLASS()
class FIUBADEFENSE_API AFDGameMode : public AGameModeBase
{
	GENERATED_BODY()



public:
	AFDGameMode();
	
	virtual void SetPlayerDefaults(APawn* PlayerPawn) override;

	virtual void StartPlay() override;

	AFDWeaponBase* CreateWeaponForPlayer(const APlayerController* PlayerController, const UFDWeaponDataAsset* WeaponDataAsset);

	FORCEINLINE int32 GetCurrentTurn() const { return CurrentTurn; }

	void AdvanceTurn();

	AFDTower* GetTower();
protected:

	void StartNewTurn();
	
	int32 CurrentTurn = 0;

	UPROPERTY(EditDefaultsOnly)
	float GoldTickTime = 1.0f;
	
	void StartMatch();
	void TickGold();
	
	bool bMatchStarted = false;
		
	void RefreshShop();

	UPROPERTY(EditDefaultsOnly)
	int32 TurnsToRefreshShop = 3;

	TWeakObjectPtr<AFDTower> PlayerTower;
};
