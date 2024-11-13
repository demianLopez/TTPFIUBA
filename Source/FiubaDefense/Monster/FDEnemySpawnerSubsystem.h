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
class FIUBADEFENSE_API UFDEnemySpawnerSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	static UFDEnemySpawnerSubsystem* Get(const UObject* WorldContextObject);
	virtual TStatId GetStatId() const override;

	void StartSpawning();
	void StopSpawning();

	virtual void Tick(float DeltaTime) override;
	
	FORCEINLINE const TArray<AFDMonster*>& GetSpawnedMosnsters() const { return SpawnedMonsters; }
protected:
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;
	float GetLastSpawnedTimeElapsed() const;
	void UpdateSpawnTimeStamp();

	void SetUpMonsterStats(AFDMonster* Monster);

	void SpawnEnemy();
	FVector FindSpawnLocation() const;
	
	void OnMonsterLoadedComplete();
	TSharedPtr<FStreamableHandle> MonstersStreamableHandle;

	UPROPERTY(Transient)
	TArray<TObjectPtr<const UFDMonsterDataAsset>> MonsterDataAssets;

	UPROPERTY(Transient)
	TArray<AFDMonster*> SpawnedMonsters;

	UPROPERTY(Transient)
	TObjectPtr<AFDGameMode> FDGameMode;

	float LastSpawnedTimeStamp = 0.0f;
	bool bActive = false;
};
