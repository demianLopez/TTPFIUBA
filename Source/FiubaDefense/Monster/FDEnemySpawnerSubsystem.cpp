// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/FDEnemySpawnerSubsystem.h"

#include "FDMonster.h"
#include "FiubaDefenseDeveloperSettings.h"
#include "Data/FDMonsterDataAsset.h"
#include "Engine/AssetManager.h"
#include "Game/FDGameMode.h"

TStatId UFDEnemySpawnerSubsystem::GetStatId() const
{
	return TStatId();
}

void UFDEnemySpawnerSubsystem::StartSpawning()
{
	bActive = true;
	FDGameMode = GetWorld()->GetAuthGameMode<AFDGameMode>();
}

void UFDEnemySpawnerSubsystem::StopSpawning()
{
	bActive = false;
}

float UFDEnemySpawnerSubsystem::GetLastSpawnedTimeElapsed() const
{
	const float WorldTimeStamp = GetWorld()->GetTimeSeconds();
	return WorldTimeStamp - LastSpawnedTimeStamp;
}

void UFDEnemySpawnerSubsystem::UpdateSpawnTimeStamp()
{
	LastSpawnedTimeStamp = GetWorld()->GetTimeSeconds();
}

UFDEnemySpawnerSubsystem* UFDEnemySpawnerSubsystem::Get(const UObject* WorldContextObject)
{
	UWorld* ContextWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);

	if(!IsValid(ContextWorld))
		return nullptr;

	return ContextWorld->GetSubsystem<UFDEnemySpawnerSubsystem>();
}

bool UFDEnemySpawnerSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void UFDEnemySpawnerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
		
	UAssetManager& AssetManager = UAssetManager::Get();

	FPrimaryAssetType MonsterType("FDMonsterDataAsset");

	TArray<FAssetData> MonstersAssetData;
	AssetManager.GetPrimaryAssetDataList(MonsterType, MonstersAssetData);

	TArray<FPrimaryAssetId> AssetIds;
	AssetIds.Reserve(MonstersAssetData.Num());

	for(const FAssetData& AssetData : MonstersAssetData)
	{
		AssetIds.Add(AssetData.GetPrimaryAssetId());
	}
	
	MonstersStreamableHandle = AssetManager.LoadPrimaryAssets(AssetIds, {"Game"}
		, FStreamableDelegate::CreateUObject(this, &ThisClass::OnMonsterLoadedComplete));
}

void UFDEnemySpawnerSubsystem::OnMonsterLoadedComplete()
{
	if(!MonstersStreamableHandle.IsValid())
		return;
	
	TArray<UObject*> LoadedMonstersAssets;
	MonstersStreamableHandle->GetLoadedAssets(LoadedMonstersAssets);

	MonsterDataAssets.Reserve(LoadedMonstersAssets.Num());
	
	for(const UObject* LoadedMonsterAsset : LoadedMonstersAssets)
	{
		const UFDMonsterDataAsset* MonsterDataAsset = Cast<const UFDMonsterDataAsset>(LoadedMonsterAsset);
		if(!IsValid(MonsterDataAsset))
			continue;
		
		MonsterDataAssets.Add(MonsterDataAsset);
	}
}



void UFDEnemySpawnerSubsystem::Tick(float DeltaTime)
{
	if(!bActive)
		return;

	const UFiubaDefenseDeveloperSettings* DefenseSettings = UFiubaDefenseDeveloperSettings::Get();
		
	const float MatchTime = FDGameMode->GetMatchElapsedTime();
	const float LastSpawnedTime = GetLastSpawnedTimeElapsed();
	const int32 Enemies = SpawnedMonsters.Num();

	const float SpawnThreshold = DefenseSettings->K1 / (1.0f + DefenseSettings->K2 * MatchTime + DefenseSettings->K3 * Enemies);

	if(LastSpawnedTime > SpawnThreshold)
	{
		SpawnEnemy();
	}
}

void UFDEnemySpawnerSubsystem::SpawnEnemy()
{
	int32 RandomMonsterIndex = FMath::RandRange(1, MonsterDataAssets.Num()) - 1;
	const UFDMonsterDataAsset* MonsterDataAsset = MonsterDataAssets[RandomMonsterIndex];

	UClass* MonsterClass = MonsterDataAsset->MonsterToSpawnClass.Get();
	if(!ensure(IsValid(MonsterClass)))
		return;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const FVector& SpawnLocation = FindSpawnLocation();
	
	AFDMonster* Monster = GetWorld()->SpawnActor<AFDMonster>(MonsterClass, SpawnLocation, FRotator::ZeroRotator, SpawnParameters);

	SetUpMonsterStats(Monster);	
	SpawnedMonsters.Add(Monster);
	
	UpdateSpawnTimeStamp();
}

FVector UFDEnemySpawnerSubsystem::FindSpawnLocation() const
{
	const UFiubaDefenseDeveloperSettings* DefenseSettings = UFiubaDefenseDeveloperSettings::Get();
	const float Distance = FMath::RandRange(DefenseSettings->SpawnMinDistance, DefenseSettings->SpawnMaxDistance);
	const float RandomAngle = FMath::RandRange(0.0f, PI * 2.0f);

	return FVector(FMath::Cos(RandomAngle), FMath::Sin(RandomAngle), 0.0f) * Distance;
}

void UFDEnemySpawnerSubsystem::SetUpMonsterStats(AFDMonster* Monster)
{
}
