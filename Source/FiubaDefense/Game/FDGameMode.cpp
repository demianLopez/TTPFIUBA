// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/FDGameMode.h"

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
	PrimaryActorTick.bCanEverTick = true;
}

void AFDGameMode::SetPlayerDefaults(APawn* PlayerPawn)
{
	Super::SetPlayerDefaults(PlayerPawn);

	AFDPlayerPawn* FDPlayerPawn = Cast<AFDPlayerPawn>(PlayerPawn);

	FVector FeetOffset = FVector::ZeroVector;
	
	const AController* Controller = PlayerPawn->GetController();
	const APlayerStart* PlayerStart = Cast<APlayerStart>(Controller->StartSpot.Get());
	if(IsValid(PlayerStart))
	{
		const UCapsuleComponent* PlayerStartCapsule = PlayerStart->GetCapsuleComponent();		
		FeetOffset += FVector(0.0f, 0.0f, -PlayerStartCapsule->GetScaledCapsuleHalfHeight());
	}
	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const AFDTower* DefaultTowerObject = GetDefault<AFDTower>(PlayerTowerClass);
	FeetOffset += DefaultTowerObject->GetFeetLocationOffset();

	const FVector& TowerSpawnLocation = FDPlayerPawn->GetActorLocation() + FeetOffset;
	const FRotator& TowerSpawnRotation = FDPlayerPawn->GetActorRotation();
	
	AFDTower* PlayerTower = GetWorld()->SpawnActor<AFDTower>(PlayerTowerClass, TowerSpawnLocation, TowerSpawnRotation, SpawnParameters);
	FDPlayerPawn->SetTowerReference(PlayerTower);
}

void AFDGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(bMatchStarted)
	{
		const float CurrentTimeStamp = GetWorld()->GetTimeSeconds();
		const float ElapsedTime = CurrentTimeStamp - LastGoldTickTimeStamp;

		if(ElapsedTime >= GoldTickTime)
		{
			TickGold();
			LastGoldTickTimeStamp = CurrentTimeStamp;
		}	
	}
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
	
	FTimerHandle TempTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TempTimerHandle, this, &ThisClass::StartMatch, 5.0f);
}

float AFDGameMode::GetMatchElapsedTime() const
{
	const float CurrentTimeSeconds = GetWorld()->GetTimeSeconds();
	return CurrentTimeSeconds - MatchStartedTime;
}

void AFDGameMode::StartMatch()
{
	MatchStartedTime = GetWorld()->GetTimeSeconds();
	LastGoldTickTimeStamp = MatchStartedTime;
	bMatchStarted = true;
	
	UFDEnemySpawnerSubsystem* SpawnerSubsystem = UFDEnemySpawnerSubsystem::Get(this);
	SpawnerSubsystem->StartSpawning();

	GetWorld()->GetTimerManager().SetTimer(ShopTimerHandle, this, &ThisClass::OnRefreshShopTimer, ShopRefreshTime, true);
	OnRefreshShopTimer();

	if(IsValid(TestWeaponData))
	{
		CreateWeaponForPlayer(GetWorld()->GetFirstPlayerController(), TestWeaponData);
	}
}

AFDWeaponBase* AFDGameMode::CreateWeaponForPlayer(const APlayerController* PlayerController,
	const UFDWeaponDataAsset* WeaponDataAsset)
{
	AFDTower* PlayerTower = UFDFunctionLibrary::GetPlayerTower(PlayerController);

	if(!IsValid(PlayerTower))
		return nullptr;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = PlayerTower;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	AFDWeaponBase* NewWeapon = GetWorld()->SpawnActor<AFDWeaponBase>(WeaponDataAsset->WeaponClass.LoadSynchronous()
		, PlayerTower->GetActorLocation(), PlayerTower->GetActorRotation(), SpawnParameters);

	NewWeapon->InitWithData(WeaponDataAsset);

	return NewWeapon;
}

void AFDGameMode::OnRefreshShopTimer()
{
	for(FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator() ; It; ++It)
	{
		AFDPlayerController* FDPlayerController = Cast<AFDPlayerController>(It->Get());
		FDPlayerController->GetShopComponent()->RefreshShop();
	}
}
