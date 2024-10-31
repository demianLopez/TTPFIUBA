// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/FDGameMode.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/PlayerStart.h"
#include "Player/FDPlayerPawn.h"
#include "Tower/FDTower.h"

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
