// Fill out your copyright notice in the Description page of Project Settings.


#include "Labyrinth/FDLabyrinthGameMode.h"

#include "FDLabyrinthInteractiveObject.h"
#include "Labyrinth/FDLabyrinthPlayer.h"
#include "FDLabyrinthObject.h"
#include "FIUBAPythonInterface.h"
#include "FPAgent.h"

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

	MaxTurns = (GridSizeX + GridSizeY) * 2;

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
	FIUBAPythonSubsystem.InitEpisode();
	FIUBAPythonSubsystem.CreateAgent("Player", 13, 4);
	
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::AdvanceTurn);
}

void AFDLabyrinthGameMode::AdvanceTurn()
{	
	TurnNumber++;

	if (TurnNumber >= MaxTurns)
	{
		bGameEnded = true;
	}
	
	AFDLabyrinthPlayer* Player = FindPlayer();
	AFDLabyrinthInteractiveObject* Door = FindInteractiveObject(EFDLabyrinthInteractiveObjectType::L_ExitDoor);

	int32 PlayerX, PlayerY, DoorX, DoorY;

	Player->GetGridLocation(PlayerX, PlayerY);
	Door->GetGridLocation(DoorX, DoorY);
	
	TArray<float> Values;

	{
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
	}

	{
		AFDLabyrinthInteractiveObject* Trophy = FindInteractiveObject(EFDLabyrinthInteractiveObjectType::L_Trophy);

		if (IsValid(Trophy))
		{
			int32 TrophyX, TrophyY;
			Trophy->GetGridLocation(TrophyX, TrophyY);
		
			int32 DeltaX = TrophyX - PlayerX;
			int32 DeltaY = TrophyY - PlayerY;

			Values.Add(DeltaX > 0 ? 1 : 0);
			Values.Add(DeltaY > 0 ? 1 : 0);
			Values.Add(DeltaX < 0 ? 1 : 0);
			Values.Add(DeltaY < 0 ? 1 : 0);

			int32 Distance = FMath::Abs(DeltaX) + FMath::Abs(DeltaY);
			Values.Add(Distance);
		}
		else
		{
			Values.Add(0);
			Values.Add(0);
			Values.Add(0);
			Values.Add(0);

			Values.Add(0);
		}
	}
			
	float Reward = 0.0f;
	if (bGameEnded && bWonGame)
	{
		Reward += bGrabbedTrophy ? 12.0f : 5.0f;
	}
	
	if (!bMoved)
	{
		Reward -= 0.1f;
	}
		
	if (bGameEnded && !bWonGame)
	{
		Reward -= 5.0f;
	}

	IFIUBAPythonInterface& FIUBAPythonSubsystem = IFIUBAPythonInterface::Get();
	UFPAgent* PlayerAgent = FIUBAPythonSubsystem.GetAgent("Player");	
	int32 TakenAction = PlayerAgent->Train(Values, Reward, bGameEnded);

	if (bGameEnded)
		return;

	if (!ensure(IsValid(Player)))
		return;
	
	// --- Action ----
	// Mover Up
	// Mover Down
	// Mover Right
	// Mover Left
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

	TArray<TWeakObjectPtr<AFDLabyrinthObject>> PendingDestroy;
	
	for(TWeakObjectPtr<AFDLabyrinthObject> DestinationObject : ObjectsAtDestination)
	{
		if (!DestinationObject->CanOverlap(Object))
			return false;

		FDOVerlapResult OverlapResult;
		DestinationObject->OnOtherObjectTryToOverlap(Object, OverlapResult);

		bGameEnded |= OverlapResult.bGameEnd;
		bWonGame |= OverlapResult.bWonGame;
		bGrabbedTrophy |= OverlapResult.bGrabbedTrophy;

		if (OverlapResult.bDestroyObject)
		{
			PendingDestroy.Add(DestinationObject);
		}
	}

	while (!PendingDestroy.IsEmpty())
	{
		TWeakObjectPtr<AFDLabyrinthObject> PendingDestroyObject = PendingDestroy.Pop();
		PendingDestroyObject->Destroy();
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

AFDLabyrinthInteractiveObject* AFDLabyrinthGameMode::FindInteractiveObject(EFDLabyrinthInteractiveObjectType Type)
{
	for (TWeakObjectPtr<AFDLabyrinthObject> Object : LabyrinthObjects)
	{
		if (!Object.IsValid())
			continue;

		AFDLabyrinthInteractiveObject* InterObject = Cast<AFDLabyrinthInteractiveObject>(Object.Get());

		if (!IsValid(InterObject))
			continue;
	
		if (InterObject->GetInteractiveObjectType() == Type)
			return InterObject;			
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