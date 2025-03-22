// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/FDGameMode.h"

#include "EngineUtils.h"
#include "FDFunctionLibrary.h"
#include "FDShopComponent.h"
#include "Components/CapsuleComponent.h"
#include "Data/FDWeaponDataAsset.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerStart.h"
#include "Monster/FDEnemySpawnerSubsystem.h"
#include "Player/FDPlayerController.h"
#include "Player/FDPlayerPawn.h"
#include "Player/FDPlayerState.h"
#include "Tower/FDTower.h"
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
}

void AFDGameMode::AdvanceTurn()
{
	PlayerTower->TickTower();
	
	UFDEnemySpawnerSubsystem* SpawnerSubsystem = UFDEnemySpawnerSubsystem::Get(this);
	SpawnerSubsystem->TickMonsters();

	StartNewTurn();
}

AFDWeaponBase* AFDGameMode::CreateWeaponForPlayer(const APlayerController* PlayerController,
                                                  const UFDWeaponDataAsset* WeaponDataAsset)
{
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
