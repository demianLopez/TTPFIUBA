// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "FDEnemySpawnerSubsystem.generated.h"

class AFDGameMode;
class AFDMonster;
class UFDMonsterDataAsset;
struct FStreamableHandle;

/**
 * 
 */
UCLASS()
class FIUBADEFENSE_API UFDEnemySpawnerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	static UFDEnemySpawnerSubsystem* Get(const UObject* WorldContextObject);

	void StartSpawning();
	void StopSpawning();

	void TickMonsters();
		
	FORCEINLINE const TArray<AFDMonster*>& GetSpawnedMosnsters() const { return SpawnedMonsters; }
protected:
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

	void UpdateSpawnTimeStamp();
	void OnMonsterKilled(AFDMonster* Monster);
	
	void TickMovement();
	void TickSpawning();

	int32 GetTotalMonsterKilled() const;

	void SetUpMonsterStats(AFDMonster* Monster);

	void SpawnEnemy();
	
	void OnMonsterLoadedComplete();
	TSharedPtr<FStreamableHandle> MonstersStreamableHandle;

	UPROPERTY(Transient)
	TArray<TObjectPtr<const UFDMonsterDataAsset>> MonsterDataAssets;

	UPROPERTY(Transient)
	TArray<AFDMonster*> SpawnedMonsters;

	UPROPERTY(Transient)
	TObjectPtr<AFDGameMode> FDGameMode;

	float LastSpawnedTurn = 0.0f;
	bool bActive = false;
};
