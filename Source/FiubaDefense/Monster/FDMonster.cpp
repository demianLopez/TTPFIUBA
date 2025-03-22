// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/FDMonster.h"

#include "Components/SphereComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Player/FDPlayerController.h"
#include "Player/FDPlayerState.h"
#include "Tower/FDTower.h"
#include "Tower/FDTowerGridComponent.h"
#include "Tower/FDTowerGridComponentTypes.h"

// Sets default values
AFDMonster::AFDMonster()
{
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = SphereComponent;
	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnMovement"));
}

void AFDMonster::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	const FVector& Velocity = GetVelocity();
	SetActorRotation(Velocity.ToOrientationRotator());
}

void AFDMonster::MoveToTile(TSharedPtr<FFDTowerGridTile> Tile)
{
	if (Tile == CurrentTile)
		return;
	
	if (CurrentTile.IsValid())
	{
		auto CurrentTileShared = CurrentTile.Pin();
		CurrentTileShared->MonstersInTile.Remove(this);
	}
	
	SetActorLocation(Tile->GetTileWorldLocation());
	Tile->MonstersInTile.Add(this);
	CurrentTile = Tile;
}

void AFDMonster::SetTargetTower(AFDTower* Tower)
{
	TargetTower = Tower;
}

float AFDMonster::TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator,
                             AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	Health -= ActualDamage;

	if(Health <= 0.0f)
	{
		OnKilled(Cast<AFDPlayerController>(EventInstigator));
	}
	
	return ActualDamage;
}

void AFDMonster::OnKilled(AFDPlayerController* KilledByPlayer)
{
	if (CurrentTile.IsValid())
	{
		CurrentTile.Pin()->MonstersInTile.Remove(this);
	}
	
	AFDPlayerState* FDPlayerState = KilledByPlayer->GetPlayerState<AFDPlayerState>();
	FDPlayerState->IncrementKillCount();
	FDPlayerState->AddGold(GoldReward);

	OnMonsterKilled.Broadcast(this);
	
	Destroy();
}

void AFDMonster::PerformAction()
{
	if (!CurrentTile.IsValid())
		return;

	auto CurrentTileShared = CurrentTile.Pin();

	int32 LocX = CurrentTileShared->TileLocation.X;
	int32 LocY = CurrentTileShared->TileLocation.Y;

	if (FMath::Abs(LocX) + FMath::Abs(LocY) <= 1)
	{
		TryAttack(TargetTower.Get());
		return;
	}
	
	FIntVector2 MovDirection;	
	MovDirection.X = LocX > 0 ? -1 : LocX < 0 ? 1 : 0;
	MovDirection.Y = LocY > 0 ? -1 : LocY < 0 ? 1 : 0;

	// Only Move in one direction, X has priority
	if (MovDirection.X != 0)
	{
		MovDirection.Y = 0;
	}

	FIntVector2 FinalLocation = CurrentTileShared->TileLocation + MovDirection;
	MoveToTile(TargetTower->GetGridComponent()->GetTileAt(FinalLocation));
}

bool AFDMonster::TryAttack(AFDTower* Tower)
{
	if(!IsValid(Tower))
		return false;

	FPointDamageEvent PointDamageEvent;
	PointDamageEvent.Damage = MonsterDamage;
		
	Tower->TakeDamage(MonsterDamage, PointDamageEvent, GetInstigatorController(), this);

	return true;
}
