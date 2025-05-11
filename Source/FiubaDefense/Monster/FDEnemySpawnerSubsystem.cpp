// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/FDEnemySpawnerSubsystem.h"

#include "FDMonster.h"
#include "FiubaDefenseDeveloperSettings.h"
#include "Data/FDMonsterDataAsset.h"
#include "Engine/AssetManager.h"
#include "Game/FDGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "Player/FDPlayerState.h"
#include "Tower/FDTower.h"
#include "Tower/FDTowerGridComponent.h"
#include "Tower/FDTowerGridComponentTypes.h"

void UFDEnemySpawnerSubsystem::StartSpawning()
{
	bActive = true;
	FDGameMode = GetWorld()->GetAuthGameMode<AFDGameMode>();
}

void UFDEnemySpawnerSubsystem::StopSpawning()
{
	bActive = false;
}

void UFDEnemySpawnerSubsystem::UpdateSpawnTimeStamp()
{
	LastSpawnedTurn = FDGameMode->GetCurrentTurn();
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
	
	MonstersStreamableHandle->WaitUntilComplete();
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

void UFDEnemySpawnerSubsystem::TickMonsters()
{
	TickMovement();
	TickSpawning();
}

void UFDEnemySpawnerSubsystem::TickMovement()
{
	for (AFDMonster* Monster : SpawnedMonsters)
	{
		Monster->PerformAction();
	}
}

void UFDEnemySpawnerSubsystem::TickSpawning()
{
	if(!bActive)
		return;

	const UFiubaDefenseDeveloperSettings* DefenseSettings = UFiubaDefenseDeveloperSettings::Get();
		
	const float CurrentTurn = FDGameMode->GetCurrentTurn();
	const float TurnsWithoutSpawning = CurrentTurn - LastSpawnedTurn;
	
	const int32 Enemies = GetTotalMonsterKilled();

	float SpawnThisTurnF = (DefenseSettings->BaseSpawn + DefenseSettings->SpawnGrowthFactor * TurnsWithoutSpawning) * (1 + CurrentTurn / DefenseSettings->TurnsConstant);
	int32 SpawnThisTurnInt = FMath::RoundToZero(SpawnThisTurnF);
	
	for (int32 i = 0; i < SpawnThisTurnInt; i++)
	{
		SpawnEnemy();
	}
}

int32 UFDEnemySpawnerSubsystem::GetTotalMonsterKilled() const
{
	const TArray<APlayerState*> PlayerArray = GetWorld()->GetGameState()->PlayerArray;

	int32 TotalMonsters = 0;
	for(const APlayerState* PlayerState : PlayerArray)
	{
		const AFDPlayerState* FDPlayerState = Cast<AFDPlayerState>(PlayerState);

		if(!IsValid(FDPlayerState))
			continue;

		TotalMonsters += FDPlayerState->GetEnemiesKilled();
	}

	return TotalMonsters;
}

void UFDEnemySpawnerSubsystem::SpawnEnemy()
{
	if (MonsterDataAssets.Num() == 0)
		return;
	
	int32 RandomMonsterIndex = FMath::RandRange(1, MonsterDataAssets.Num()) - 1;
	const UFDMonsterDataAsset* MonsterDataAsset = MonsterDataAssets[RandomMonsterIndex];

	UClass* MonsterClass = MonsterDataAsset->MonsterToSpawnClass.Get();
	if(!ensure(IsValid(MonsterClass)))
		return;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AFDTower* Tower = FDGameMode->GetTower();
	UFDTowerGridComponent* GridComponent = Tower->GetGridComponent();

	TArray<TSharedPtr<FFDTowerGridTile>> BorderTiles;
	GridComponent->GetBorderTiles(BorderTiles);

	TSharedPtr<FFDTowerGridTile> RandomBorder = BorderTiles[FMath::RandRange(0, BorderTiles.Num() - 1)];
	
	AFDMonster* Monster = GetWorld()->SpawnActor<AFDMonster>(MonsterClass, RandomBorder->GetTileWorldLocation(), FRotator::ZeroRotator, SpawnParameters);
	Monster->MoveToTile(RandomBorder);

	Monster->OnMonsterKilled.AddUObject(this, &ThisClass::OnMonsterKilled);
	
	SetUpMonsterStats(Monster);	
	SpawnedMonsters.Add(Monster);

	Monster->SetTargetTower(Tower);
	
	UpdateSpawnTimeStamp();
}

void UFDEnemySpawnerSubsystem::OnMonsterKilled(AFDMonster* Monster)
{
	Monster->OnMonsterKilled.RemoveAll(this);
	SpawnedMonsters.Remove(Monster);
}

void UFDEnemySpawnerSubsystem::SetUpMonsterStats(AFDMonster* Monster)
{
}
