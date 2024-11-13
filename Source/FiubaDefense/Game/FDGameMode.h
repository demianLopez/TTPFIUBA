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

	float GetMatchElapsedTime() const;
	
	virtual void Tick(float DeltaSeconds) override;

	AFDWeaponBase* CreateWeaponForPlayer(const APlayerController* PlayerController, const UFDWeaponDataAsset* WeaponDataAsset);
protected:

	float LastGoldTickTimeStamp = 0.0f;

	UPROPERTY(EditDefaultsOnly)
	float GoldTickTime = 1.0f;
	
	void StartMatch();
	void TickGold();

	float MatchStartedTime;
	bool bMatchStarted = false;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AFDTower> PlayerTowerClass;

	FTimerHandle ShopTimerHandle;
	void OnRefreshShopTimer();

	UPROPERTY(EditDefaultsOnly)
	float ShopRefreshTime = 30.0f;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UFDWeaponDataAsset> TestWeaponData;
};
