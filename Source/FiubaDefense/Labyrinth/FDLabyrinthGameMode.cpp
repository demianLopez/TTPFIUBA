// Fill out your copyright notice in the Description page of Project Settings.


#include "Labyrinth/FDLabyrinthGameMode.h"

#include "FDLabyrinthInteractiveObject.h"
#include "Labyrinth/FDLabyrinthPlayer.h"
#include "FDLabyrinthObject.h"
#include "FIUBAPythonInterface.h"
#include "FPAgent.h"

int32 AFDLabyrinthGameMode::TotalEnemyWon = 0;
int32 AFDLabyrinthGameMode::TotalPlayerWon = 0;

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

	MaxTurns = 30;

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
	FIUBAPythonSubsystem.CreateAgent("Player", 16, 5);
	FIUBAPythonSubsystem.CreateAgent("Enemy", 16, 5);
	FIUBAPythonSubsystem.CreateAgent("Door", 18, 5);

	UE_LOG(LogTemp, Warning, TEXT("Games Enemy %d vs %d Player"), TotalEnemyWon, TotalPlayerWon);
	
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::AdvanceTurn);
}

void AFDLabyrinthGameMode::AdvanceTurn()
{
	IFIUBAPythonInterface& FIUBAPythonSubsystem = IFIUBAPythonInterface::Get();

	TurnNumber++;

	if (TurnNumber >= MaxTurns)
	{
		UE_LOG(LogTemp, Warning, TEXT("Out Of Movements!"));

		bGameEnded = true;
	}
	
	AFDLabyrinthPlayer* Player = FindPlayer();
	AFDLabyrinthObject* Enemy = FindInteractiveObject(EFDLabyrinthInteractiveObjectType::L_Enemy);
	AFDLabyrinthObject* Door = FindInteractiveObject(EFDLabyrinthInteractiveObjectType::L_ExitDoor);

	if (!bEndedForPlayer)
	{
		int32 PlayerAction = 0;	
		{
			TArray<float> PlayerState;
			
			AppendBorders(Player, PlayerState);
			Enemy->AppendDirectionToObject(Player, PlayerState);
			Door->AppendDirectionToObject(Player, PlayerState);
			
			float PlayerReward = 0.0f;
	
			if (bGameEnded && bWonGame)
			{
				PlayerReward += 6.0f;
			}
					
			if (bGameEnded && !bWonGame)
			{
				PlayerReward -= 6.0f;
			}

			UFPAgent* PlayerAgent = FIUBAPythonSubsystem.GetAgent("Player");	
			PlayerAction = PlayerAgent->Train(PlayerState, PlayerReward, bGameEnded);
			bEndedForPlayer = bGameEnded;
		}		
	
		if (!bEndedForPlayer)
		{
			int32 DeltaX = 0, DeltaY = 0;
			GetDeltaFromMovementAction(PlayerAction, DeltaX, DeltaY);
			GridTryMoveDeltaObject(Player, DeltaX, DeltaY);
		}
	}
	// Enemy! -----------------------

	if (!bEndedForEnemy)
	{
		int32 EnemyAction = 0;
		{
			TArray<float> EnemyState;
			AppendBorders(Enemy, EnemyState);
			Player->AppendDirectionToObject(Enemy, EnemyState);
			Door->AppendDirectionToObject(Enemy, EnemyState);
			
			float EnemyReward = 0.0f;

			if (bGameEnded && bWonGame)
			{
				EnemyReward -= 6.0f;
			}

			if (bGameEnded && !bWonGame)
			{
				EnemyReward += bPlayerKilled ? 10.0f : 1.0f;
			}

			UFPAgent* EnemyAgent = FIUBAPythonSubsystem.GetAgent("Enemy");	
			EnemyAction = EnemyAgent->Train(EnemyState, EnemyReward, bGameEnded);
			bEndedForEnemy = bGameEnded;
		}

		if (!bEndedForEnemy)
		{
			int32 DeltaX = 0, DeltaY = 0;
			GetDeltaFromMovementAction(EnemyAction, DeltaX, DeltaY);
			GridTryMoveDeltaObject(Enemy, DeltaX, DeltaY);
		}
	}

	if (!bEndedForDoor)
	{
		int32 DoorAction = 0;
		{
			TArray<float> DoorState;
			AppendBorders(Door, DoorState);
			Player->AppendDirectionToObject(Door, DoorState);
			Enemy->AppendDirectionToObject(Player, DoorState);
			//AppendBinaryToFloatArray(TurnNumber, 4, DoorState);

			if (TotalEnemyWon > TotalPlayerWon)
			{
				DoorState.Add(1.0f); DoorState.Add(0.0f);
			}
			else if (TotalPlayerWon < TotalEnemyWon)
			{
				DoorState.Add(0.0f); DoorState.Add(1.0f);
			}
			else
			{
				DoorState.Add(0.0f); DoorState.Add(0.0f);
			}
			
			float DoorReward = 0.0f;

			bool bPlayerWon = bGameEnded && bWonGame;
			bool bEnemyWon = bGameEnded && !bWonGame;

			if (bPlayerWon)
			{
				TotalPlayerWon++;
				TotalPlayerWon = FMath::Min(TotalPlayerWon, TotalEnemyWon + 2);
			}

			if (bEnemyWon)
			{
				TotalEnemyWon++;
				TotalEnemyWon = FMath::Min(TotalEnemyWon, TotalPlayerWon + 2);
			}
			
			bool bBalanced = FMath::Abs(TotalEnemyWon - TotalPlayerWon) < 2;		
			
			if (bGameEnded)
			{
				UE_LOG(LogTemp, Warning, TEXT("Balanced: %s"), bBalanced ? TEXT("True") : TEXT("False"));

				DoorReward += bBalanced ? 6.0f : -6.0f;
			}
			
			UFPAgent* DoorAgent = FIUBAPythonSubsystem.GetAgent("Door");	
			DoorAction = DoorAgent->Train(DoorState, DoorReward, bGameEnded);
			bEndedForDoor = bGameEnded;
		}

		if (!bEndedForDoor)
		{
			int32 DeltaX = 0, DeltaY = 0;
			GetDeltaFromMovementAction(DoorAction, DeltaX, DeltaY);
			GridTryMoveDeltaObject(Door, DeltaX, DeltaY);
		}
	}
	
	if (bEndedForEnemy && bEndedForPlayer && bEndedForDoor)
		return;

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

void AFDLabyrinthGameMode::AppendBorders(AFDLabyrinthObject* Object, TArray<float>& OutBorders)
{
	int32 PlayerX, PlayerY;
	Object->GetGridLocation(PlayerX, PlayerY);
	
	OutBorders.Add(PlayerX == 0 ? 1 : 0);
	OutBorders.Add(PlayerY == 0 ? 1 : 0);
	OutBorders.Add(PlayerX == (GridSizeX - 1) ? 1 : 0);
	OutBorders.Add(PlayerY == (GridSizeY - 1) ? 1 : 0);	
}

bool AFDLabyrinthGameMode::GridTryMoveDeltaObject(AFDLabyrinthObject* Object, int32 DeltaX, int32 DeltaY)
{
	if (DeltaX == 0 && DeltaY == 0)
		return false;
	
	int32 CurrentLocX, CurrentLocY;
	Object->GetGridLocation(CurrentLocX, CurrentLocY);

	int32 FinalX = CurrentLocX + DeltaX;
	int32 FinalY = CurrentLocY + DeltaY;

	return GridTryMoveObject(Object, FinalX, FinalY);
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
		bPlayerKilled |= OverlapResult.bPlayerKilled;
		
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

void AFDLabyrinthGameMode::GetDeltaFromMovementAction(int32 Action, int32& OutDeltaX, int32& OutDeltaY)
{
	OutDeltaX = 0;
	OutDeltaY = 0;
	
	switch (Action)
	{
	default:
	case 0: // Mover Up
		OutDeltaX = 1;
		break;
	case 1: // Mover Down
		OutDeltaX = -1;
		break;
	case 2: // Mover Right
		OutDeltaY = 1;
		break;
	case 3: // Mover Left
		OutDeltaY = -1;
		break;
	case 4:
		break;
	}
}

void AFDLabyrinthGameMode::AppendBinaryToFloatArray(int32 Number, int32 NumDigits, TArray<float>& OutArray)
{
	int32 MaxValue = (1 << NumDigits) - 1;
	if (Number > MaxValue)
	{
		for (int32 i = 0; i < NumDigits; ++i)
		{
			OutArray.Add(1.0f);
		}

		return;
	}
	
	for (int32 i = NumDigits - 1; i >= 0; --i)
	{
		int32 Bit = (Number >> i) & 1;
		OutArray.Add(static_cast<float>(Bit));
	}
}
