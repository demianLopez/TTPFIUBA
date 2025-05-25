// Fill out your copyright notice in the Description page of Project Settings.


#include "Labyrinth/FDLabyrinthGameMode.h"

#include "FDLabyrinthInteractiveObject.h"
#include "Labyrinth/FDLabyrinthPlayer.h"
#include "FDLabyrinthObject.h"
#include "FIUBAPythonInterface.h"

bool FLabyrinthGridData::IsChannelOccupied(int32 Channel)
{
	for (TWeakObjectPtr<AFDLabyrinthObject> Object : Objects)
	{
		if (!Object.IsValid())
			continue;

		if (Channel == Object->GetIdentifier())
			return true;
	}

	return false;
}

void AFDLabyrinthGameMode::StartPlay()
{
	Super::StartPlay();

	int32 TotalMapSize = GridSizeX * GridSizeY;
	LabyrinthGrid.AddDefaulted(TotalMapSize);

	MaxTurns = (GridSizeX + GridSizeY) * 1.5;
	MaxTurns = 20;

	for (int X = 0; X < GridSizeX; X++)
	{
		for (int Y = 0; Y < GridSizeY; Y++)
		{
			int32 GridIndex = GetGridIndex(X, Y);
			FLabyrinthGridData& GridData = LabyrinthGrid[GridIndex];

			GridData.X = X;
			GridData.Y = Y;
		}
	}

	// Spawn Randoms Objects
	FRandomStream RandomStream(FMath::Rand32());
	
	for (const TSubclassOf<AFDLabyrinthObject>& ObjectClass : RandomObjects)
	{
		if (!IsValid(ObjectClass))
			continue;
		
		SpawnObjectRandomly(ObjectClass, RandomStream);
	}

	DrawGrid();

	
	// Valores
	// 100 slots del mapa
	
	// --- Action ----
	// Mover Up
	// Mover Down
	// Mover Right
	// Mover Left
	
	IFIUBAPythonInterface& FIUBAPythonSubsystem = IFIUBAPythonInterface::Get();
	TArray<float> Values;
	Values.AddDefaulted(8);
	
	FIUBAPythonSubsystem.InitializeTrain(Values, 4);
	
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::AdvanceTurn);
}

void AFDLabyrinthGameMode::AdvanceTurn()
{
	TurnNumber++;

	if (TurnNumber >= MaxTurns)
	{
		bGameEnded = true;
	}
	
	IFIUBAPythonInterface& FIUBAPythonSubsystem = IFIUBAPythonInterface::Get();

	int32 TotalMapSize = GridSizeX * GridSizeY;

	AFDLabyrinthPlayer* Player = FindPlayer();
	AFDLabyrinthInteractiveObject* Door = FindObject<AFDLabyrinthInteractiveObject>();

	int32 PlayerX, PlayerY, DoorX, DoorY;

	Player->GetGridLocation(PlayerX, PlayerY);
	Door->GetGridLocation(DoorX, DoorY);
	
	TArray<float> Values;

	int32 DeltaX = DoorX - PlayerX;
	int32 DeltaY = DoorY - PlayerY;

	Values.Add(DeltaX > 0 ? 1 : 0);
	Values.Add(DeltaY > 0 ? 1 : 0);
	Values.Add(DeltaX < 0 ? 1 : 0);
	Values.Add(DeltaY < 0 ? 1 : 0);

	Values.Add(PlayerX == 0 ? 1 : 0);
	Values.Add(PlayerY == 0 ? 1 : 0);
	Values.Add(PlayerX == (GridSizeX - 1) ? 1 : 0);
	Values.Add(PlayerY == (GridSizeY - 1) ? 1 : 0);
	
	/*
	for (int X = 0; X < GridSizeX; X++)
	{
		for (int Y = 0; Y < GridSizeY; Y++)
		{
			int32 GridIndex = GetGridIndex(X, Y);
			FLabyrinthGridData& GridData = LabyrinthGrid[GridIndex];
			
			for (int Channel = 0; Channel < 2; Channel++)
			{
				int32 ChannelOffset = Channel * TotalMapSize;
				bool bIsChannelOccupied = GridData.IsChannelOccupied(Channel);
				
				Values[ChannelOffset + GridIndex] = bIsChannelOccupied ? 1.0f : 0.0f;
			}
		}
	}
	*/
	
	/*
	for (int32 Index = 0; Index < GridSizeX * GridSizeY; Index++)
	{
		TWeakObjectPtr<AFDLabyrinthObject> GridObject = LabyrinthGrid[Index];

		int32 Identifier = GridObject.IsValid() ? GridObject->GetIdentifier() : 0;
		Values[Index] = Identifier;
	}*/

	// --- Action ----
	// Mover Up
	// Mover Down
	// Mover Right
	// Mover Left
		
	float Reward = 0.0f;
	if (bGameEnded && bWonGame)
	{
		Reward += 5.0f;
	}

	float DisSquared = FVector::DistSquared(FVector(PlayerX, PlayerY, 0.0f), FVector(DoorX, DoorY, 0.0f));
	//UE_LOG(LogTemp, Warning, TEXT("LastDist %.2f NewDist %.2f"), LastDistSquared, DisSquared);

	if (!FMath::IsNearlyZero(LastDistSquared))
	{
		bool bIsNear = DisSquared < LastDistSquared;
		if (bIsNear)
		{		
			Reward += 0.1f;
		}
		else
		{
			Reward -= bMoved ? 0.1f : 1.0f;
		}
	}
	
	LastDistSquared = DisSquared;

	/*if (!bGameEnded)
	{
		FFIRewardValues PointsMinValues;
		PointsMinValues.Value = 1.0f;

		MinValues.Add(PointsMinValues);
	}
	*/
	
	if (bGameEnded && !bWonGame)
	{
		Reward -= 5.0f;
	}

	bool bPositive = Reward > 0.0;

	UE_LOG(LogTemp, Warning, TEXT("Reward %s"), bPositive ? TEXT("Positive") : TEXT("Negative"));
	
	int32 TakenAction = FIUBAPythonSubsystem.Train(Values, Reward ,false, bGameEnded);

	if (bGameEnded)	
		return;

	if (!ensure(IsValid(Player)))
		return;

	UE_LOG(LogTemp, Warning, TEXT("Action %d"), TakenAction);

	int32 MoveX = 0, MoveY = 0;
	switch (TakenAction)
	{
		default:
		case 0:
			MoveX = 1;
			break;
		case 1:
			MoveX = -1;
			break;
		case 2:
			MoveY = 1;
			break;
		case 3:
			MoveY = -1;
			break;
	}

	int32 CurrentLocX, CurrentLocY;
	Player->GetGridLocation(CurrentLocX, CurrentLocY);

	int32 FinalX = CurrentLocX + MoveX;
	int32 FinalY = CurrentLocY + MoveY;
	
	bMoved = GridTryMoveObject(Player, FinalX, FinalY);

	FTimerHandle Handle;
	float TimeBetweenRounds = FIUBAPythonSubsystem.GetTimeBetweenRounds();
	if (FMath::IsNearlyZero(TimeBetweenRounds))
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::AdvanceTurn);
	}
	else
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::AdvanceTurn, TimeBetweenRounds);
	}
}

bool AFDLabyrinthGameMode::GridTryMoveObject(AFDLabyrinthObject* Object, int32 DestX, int32 DestY)
{
	if (!IsValid(Object))
		return false;

	if (DestX >= GridSizeX || DestY >= GridSizeY || DestX < 0 || DestY < 0)
		return false;

	const TArray<TWeakObjectPtr<AFDLabyrinthObject>>& ObjectsAtDestination = GetObjectsAt(DestX, DestY);
	for(TWeakObjectPtr<AFDLabyrinthObject> DestinationObject : ObjectsAtDestination)
	{
		if (!DestinationObject->CanOverlap(Object))
			return false;

		FDOVerlapResult OverlapResult;
		DestinationObject->OnOtherObjectTryToOverlap(Object, OverlapResult);

		bGameEnded |= OverlapResult.bGameEnd;
		bWonGame |= OverlapResult.bWonGame;
	}

	int32 FromX, FromY;
	Object->GetGridLocation(FromX, FromY);

	// Remove it from current
	int32 FromGridIndex = GetGridIndex(FromX, FromY);	
	LabyrinthGrid[FromGridIndex].Objects.Remove(Object);

	int32 DestGridIndex = GetGridIndex(DestX, DestY);
	LabyrinthGrid[DestGridIndex].Objects.Add(Object);

	const FVector DestLocation = GetTileWorldLocation(DestX, DestY);
	Object->SetActorLocation(DestLocation);
	Object->SetGridLocation(DestX, DestY);
	
	return true;
}

void AFDLabyrinthGameMode::DrawGrid()
{
	for (int32 X = 0; X < GridSizeX; X++)
	{
		for (int32 Y = 0; Y < GridSizeY; Y++)
		{
			FVector Location = GetTileWorldLocation(X, Y);
			FVector Size = FVector(TileDimension, TileDimension, 50.0f) * 0.5;
			DrawDebugBox(GetWorld(), Location, Size, FColor::Red, true);
		}
	}
}

AFDLabyrinthObject* AFDLabyrinthGameMode::SpawnObjectRandomly(TSubclassOf<AFDLabyrinthObject> ObjectClass, FRandomStream& RandomStream)
{
	AFDLabyrinthObject* SpawnedObject = nullptr;

	while (!IsValid(SpawnedObject))
	{
		int32 RandomX =	RandomStream.RandRange(0, GridSizeX - 1);
		int32 RandomY =	RandomStream.RandRange(0, GridSizeY - 1);

		SpawnedObject = SpawnObjectAt(ObjectClass, RandomX, RandomY, true);		
	}

	return SpawnedObject;
}

AFDLabyrinthObject* AFDLabyrinthGameMode::SpawnObjectAt(TSubclassOf<AFDLabyrinthObject> ObjectClass, int32 X, int32 Y, bool bSpawnAlone)
{
	int32 GridIndex = GetGridIndex(X, Y);

	if (bSpawnAlone)
	{
		const TArray<TWeakObjectPtr<AFDLabyrinthObject>>& ObjectsAtDestination = GetObjectsAt(X, Y);

		if (!ObjectsAtDestination.IsEmpty())
			return nullptr;
	}
	
	AFDLabyrinthObject* NewActor = GetWorld()->SpawnActor<AFDLabyrinthObject>(ObjectClass);

	const FVector SpawnLocation = GetTileWorldLocation(X, Y);
	NewActor->SetActorLocation(SpawnLocation);
	NewActor->SetGridLocation(X, Y);
	
	FLabyrinthGridData& GridData = LabyrinthGrid[GridIndex];
	GridData.Objects.Add(NewActor);
	
	LabyrinthObjects.Add(NewActor);

	return NewActor;
}

FVector AFDLabyrinthGameMode::GetTileWorldLocation(int32 X, int32 Y)
{
	const FVector HalfTileSize = FVector(TileDimension, TileDimension, 0.0f) * 0.5f;
	return FVector(X * TileDimension, Y * TileDimension, 0) + HalfTileSize;
}

int32 AFDLabyrinthGameMode::GetGridIndex(int32 X, int32 Y)
{
	return Y * GridSizeX + X;
}

void AFDLabyrinthGameMode::GetGridCoordinate(int32 Index, int32& OutX, int32& OutY)
{
	OutX = Index % GridSizeX;
	OutY = Index / GridSizeX;
}

const TArray<TWeakObjectPtr<AFDLabyrinthObject>>& AFDLabyrinthGameMode::GetLabyrinthObjects(int32 Index)
{
	return LabyrinthGrid[Index].Objects;
}

AFDLabyrinthObject* AFDLabyrinthGameMode::FindObject(TSubclassOf<AFDLabyrinthObject> ObjectClass)
{
	for (TWeakObjectPtr<AFDLabyrinthObject> Object : LabyrinthObjects)
	{
		if (!Object.IsValid())
			continue;

		if(Object->IsA(ObjectClass))
			return Object.Get();
	}

	return nullptr;
}

AFDLabyrinthPlayer* AFDLabyrinthGameMode::FindPlayer()
{
	if (CachedPlayer.IsValid())
	{
		return CachedPlayer.Get();
	}
	
	CachedPlayer = FindObject<AFDLabyrinthPlayer>();

	if (CachedPlayer.IsValid())
		return CachedPlayer.Get();

	return nullptr;
}

const TArray<TWeakObjectPtr<AFDLabyrinthObject>>& AFDLabyrinthGameMode::GetObjectsAt(int32 X, int32 Y)
{
	const int32 GridIndex = GetGridIndex(X, Y);
	return GetLabyrinthObjects(GridIndex);
}

void AFDLabyrinthGameMode::ObjectRemoved(AFDLabyrinthObject* Object)
{
	int32 PosX, PosY;
	Object->GetGridLocation(PosX, PosY);

	int32 GridIndex = GetGridIndex(PosX, PosY);
	
	LabyrinthGrid[GridIndex].Objects.Remove(Object);
	LabyrinthObjects.Remove(Object);
}