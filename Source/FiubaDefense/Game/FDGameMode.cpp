// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/FDGameMode.h"

#include "EngineUtils.h"
#include "FDFunctionLibrary.h"
#include "FDShopComponent.h"
#include "FiubaDefenseAISettings.h"
#include "FIUBAPythonInterface.h"
#include "Components/CapsuleComponent.h"
#include "Data/FDWeaponDataAsset.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerStart.h"
#include "Monster/FDEnemySpawnerSubsystem.h"
#include "Player/FDPlayerController.h"
#include "Player/FDPlayerPawn.h"
#include "Player/FDPlayerState.h"
#include "Tower/FDTower.h"
#include "Tower/FDTowerGridComponent.h"
#include "Tower/FDTowerGridComponentTypes.h"
#include "Weapons/FDWeaponBase.h"

AFDGameMode::AFDGameMode()
{

}

void AFDGameMode::SetPlayerDefaults(APawn* PlayerPawn)
{
	Super::SetPlayerDefaults(PlayerPawn);

	AFDPlayerPawn* FDPlayerPawn = Cast<AFDPlayerPawn>(PlayerPawn);
	
	AFDTower* AvailableTower = nullptr;
	for (TActorIterator<AFDTower> It(GetWorld()); It; ++It)
	{
		AFDTower* Tower = *It;

		if (!IsValid(Tower))
			continue;

		AvailableTower = Tower;
		break;
	}

	AvailableTower->SetOwner(PlayerPawn->GetController());
	FDPlayerPawn->SetTowerReference(AvailableTower);
	PlayerTower = AvailableTower;
}

AFDTower* AFDGameMode::GetTower()
{
	return PlayerTower.Get();
}

void AFDGameMode::TickGold()
{
	AGameStateBase* GameStateBase = GetGameState<AGameStateBase>();

	for(APlayerState* PlayerState : GameStateBase->PlayerArray)
	{
		AFDPlayerState* FDPlayerState = Cast<AFDPlayerState>(PlayerState);
		FDPlayerState->IncrementIncome();
	}
}

void AFDGameMode::StartPlay()
{
	Super::StartPlay();
	
	StartMatch();

	IFIUBAPythonInterface& FIUBAPythonSubsystem = IFIUBAPythonInterface::Get();

	TArray<float> Values;
	Values.AddDefaulted(128);

	// Valores
	// 121 Slots del mapa - 121
	// Oro
	// Vida Torre
	// Precio Arma 1
	// Precio arma 2
	// Puntos
	// Enemigos matados
	// Armas compradas
	
	// --- Action ----
	// Atacar
	// Comprar arma 1
	// Comprar arma 2
	
	FIUBAPythonSubsystem.InitializeTrain(Values, 3);
}

void AFDGameMode::StartMatch()
{
	bMatchStarted = true;
	RefreshShop();
	
	UFDEnemySpawnerSubsystem* SpawnerSubsystem = UFDEnemySpawnerSubsystem::Get(this);
	SpawnerSubsystem->StartSpawning();
	
	StartNewTurn();
}

void AFDGameMode::StartNewTurn()
{
	CurrentTurn++;

	if ((CurrentTurn % TurnsToRefreshShop) == 0)
	{
		RefreshShop();
	}

	TickGold();

	const UFiubaDefenseAISettings* AISettings = UFiubaDefenseAISettings::Get();

	if (FMath::IsNearlyZero(AISettings->DelayBetweenRounds))
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::AdvanceTurn);
	}
	else
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::AdvanceTurn, AISettings->DelayBetweenRounds);
	}
}

void AFDGameMode::AdvanceTurn()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	
	IFIUBAPythonInterface& FIUBAPythonSubsystem = IFIUBAPythonInterface::Get();
	TArray<float> Values;
	Values.AddDefaulted(121);

	AFDTower* Tower = UFDFunctionLibrary::GetPlayerTower(PlayerController);

	UFDTowerGridComponent* GridComponent = Tower->GetGridComponent();
	FIntVector2 GridSize = GridComponent->GetGridSize();
	FIntVector2 GridRadius = GridComponent->GetGridRadius();
	
	for (auto Tile : GridComponent->GetTowerGridTiles())
	{
		int32 TileX = Tile->TileLocation.X + GridRadius.X;
		int32 TileY = Tile->TileLocation.Y + GridRadius.Y;
		
		int32 Index = TileX + TileY * GridSize.X;
		Values[Index] = Tile->MonstersInTile.Num();
	}

	AFDPlayerState* PlayerState = PlayerController->GetPlayerState<AFDPlayerState>();
	Values.Add(PlayerState->GetGold());
	Values.Add(Tower->GetCurrentHealth());
	Values.Add(Weapon1->BasePrice);
	Values.Add(Weapon2->BasePrice);
	Values.Add(CurrentTurn);
	Values.Add(PlayerState->GetEnemiesKilled());
	Values.Add(Tower->GetWeaponNumbers());
	
	FFIRewardValues MaxValueCurrentTurn;
	MaxValueCurrentTurn.Value = CurrentTurn;

	FFIRewardValues MaxValueEnemiesKilled;
	MaxValueEnemiesKilled.Value = PlayerState->GetEnemiesKilled();

	FFIRewardValues MinValueDead;
	MinValueDead.Value = 20.0f;

	bool bFinishLoop = Tower->GetCurrentHealth() <= 0.0f;
	int32 TakenAction = 0 /*FIUBAPythonSubsystem.Train(Values, { MaxValueCurrentTurn, MaxValueEnemiesKilled }, { MinValueDead } ,false, bFinishLoop)*/;

	if (bFinishLoop)
		return;
	
	UE_LOG(LogTemp, Log, TEXT("TakenAction: %d"), TakenAction);

	if (TakenAction == 0)
	{
		PlayerTower->TickTower();
	}
	else if (TakenAction == 1)
	{
		CreateWeaponForPlayer(PlayerController, Weapon1);	
	}
	else if (TakenAction == 2)
	{
		CreateWeaponForPlayer(PlayerController, Weapon2);
	}
	
	UFDEnemySpawnerSubsystem* SpawnerSubsystem = UFDEnemySpawnerSubsystem::Get(this);
	SpawnerSubsystem->TickMonsters();

	StartNewTurn();
}

AFDWeaponBase* AFDGameMode::CreateWeaponForPlayer(const APlayerController* PlayerController,
                                                  const UFDWeaponDataAsset* WeaponDataAsset)
{
	AFDPlayerState* OwningPlayerState = PlayerController->GetPlayerState<AFDPlayerState>();

	if(OwningPlayerState->GetGold() < WeaponDataAsset->BasePrice)
		return nullptr;

	OwningPlayerState->SubstractGold(WeaponDataAsset->BasePrice);
	
	AFDTower* Tower = UFDFunctionLibrary::GetPlayerTower(PlayerController);

	if(!IsValid(Tower))
		return nullptr;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = Tower;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	AFDWeaponBase* NewWeapon = GetWorld()->SpawnActor<AFDWeaponBase>(WeaponDataAsset->WeaponClass.LoadSynchronous()
		, Tower->GetActorLocation(), Tower->GetActorRotation(), SpawnParameters);

	NewWeapon->InitWithData(WeaponDataAsset);
	Tower->AddWeapon(NewWeapon);

	return NewWeapon;
}

void AFDGameMode::RefreshShop()
{
	for(FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator() ; It; ++It)
	{
		AFDPlayerController* FDPlayerController = Cast<AFDPlayerController>(It->Get());
		FDPlayerController->GetShopComponent()->RefreshShop();
	}
}
