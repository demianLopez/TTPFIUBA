// Fill out your copyright notice in the Description page of Project Settings.


#include "Labyrinth/FDLabyrinthGameMode.h"

#include "FDLabyrinthInteractiveObject.h"
#include "Labyrinth/FDLabyrinthPlayer.h"
#include "FDLabyrinthObject.h"
#include "FIUBAPythonInterface.h"
#include "FPAgent.h"

int32 AFDLabyrinthGameMode::TotalEnemyWon = 0;
int32 AFDLabyrinthGameMode::TotalPlayerWon = 0;
int32 AFDLabyrinthGameMode::TotalGlobalEpisodes = 0;

void AFDLabyrinthGameMode::StartPlay()
{
	Super::StartPlay();
	
	int32 TotalMapSize = GridSizeX * GridSizeY;
	LabyrinthGrid.AddDefaulted(TotalMapSize);

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

	DrawGrid();
	
	IFIUBAPythonInterface& FIUBAPythonSubsystem = IFIUBAPythonInterface::Get();
	FIUBAPythonSubsystem.InitEpisode();
	FIUBAPythonSubsystem.CreateAgent("Player", 24, 5);

	bool bIsDoingAutonomousTraining = FIUBAPythonSubsystem.IsPerformingAutonomousTraining();
	if (bIsDoingAutonomousTraining)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::PlayMultipleMatches);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::RestartMatch);
	}
}

void AFDLabyrinthGameMode::PlayMultipleMatches()
{
	IFIUBAPythonInterface& FIUBAPythonSubsystem = IFIUBAPythonInterface::Get();

	int32 RemainingToRest = 100;
	while (FIUBAPythonSubsystem.HasPendingMatch() && RemainingToRest > 0)
	{
		RestartMatch();
		RemainingToRest--;
		TotalEpisodes++;
		TotalGlobalEpisodes++;
	}
	
	UE_LOG(LogTemp, Display, TEXT("Total Episodes: %d"), TotalEpisodes);

	if (FIUBAPythonSubsystem.HasPendingMatch())
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::PlayMultipleMatches);
	}
}

void AFDLabyrinthGameMode::RestartMatch()
{
	TArray<TWeakObjectPtr<AFDLabyrinthObject>> Copy = LabyrinthObjects;

	for (auto Object : Copy)
	{
		DestroyObject(Object.Get());
	}
	
	ensure(LabyrinthObjects.IsEmpty());
	
	// Spawn Randoms Objects
	FRandomStream RandomStream(FMath::Rand32());
	
	for (const FMSpawnObjectSettings& ObjectSetting : RandomObjects)
	{
		if (ObjectSetting.SpawnAfterMatch > TotalGlobalEpisodes)
			continue;
		
		if (!IsValid(ObjectSetting.SpawnObjectClass))
			continue;
		
		SpawnObjectRandomly(ObjectSetting.SpawnObjectClass, RandomStream);
	}

	TurnNumber = 0;
	
	bEndedForPlayer = false;
	bEndedForEnemy = false;
	bEndedForDoor = false;
	bEnemyCanMove = false;
	
	GameOverlapResult.Reset();
	GameOverlapResult = MakeShared<FDOVerlapResult>();

	IFIUBAPythonInterface& FIUBAPythonSubsystem = IFIUBAPythonInterface::Get();
	FIUBAPythonSubsystem.InitEpisode();

	AdvanceTurn();
}

void AFDLabyrinthGameMode::AdvanceTurn()
{
	IFIUBAPythonInterface& FIUBAPythonSubsystem = IFIUBAPythonInterface::Get();
	bool bIsDoingAutonomousTraining = FIUBAPythonSubsystem.IsPerformingAutonomousTraining();

	bool bMatchEnded = false;
	do
	{
		bMatchEnded = Internal_AdvanceTurn();
	}
	while (!bMatchEnded && bIsDoingAutonomousTraining);

	if (!bIsDoingAutonomousTraining)
	{
		if (bMatchEnded)
		{
			UE_LOG(LogTemp, Display, TEXT("Match Ended"));
		}
		else
		{
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::AdvanceTurn, 0.4f);
		}
	}
}

bool AFDLabyrinthGameMode::Internal_AdvanceTurn()
{
	IFIUBAPythonInterface& FIUBAPythonSubsystem = IFIUBAPythonInterface::Get();

	TurnNumber++;

	if (TurnNumber >= MaxTurns)
	{
		GameOverlapResult->bGameEnd = true;
	}
	
	AFDLabyrinthObject* Player = FindInteractiveObject(EFDLabyrinthInteractiveObjectType::L_Player);
	AFDLabyrinthObject* Enemy = FindInteractiveObject(EFDLabyrinthInteractiveObjectType::L_Enemy);
	AFDLabyrinthObject* Door = FindInteractiveObject(EFDLabyrinthInteractiveObjectType::L_ExitDoor);
	AFDLabyrinthObject* Trophy = FindInteractiveObject(EFDLabyrinthInteractiveObjectType::L_Trophy);
	AFDLabyrinthObject* Key = FindInteractiveObject(EFDLabyrinthInteractiveObjectType::L_Key);

	if (!bEndedForPlayer)
	{
		int32 PlayerAction = 0;	
		{
			TArray<float> PlayerState;
			
			AppendBorders(Player, PlayerState);
			Player->AppendDirectionToObject(Enemy, PlayerState);
			Player->AppendDirectionToObject(Door, PlayerState, false);
			Player->AppendDirectionToObject(Key, PlayerState, false);
			Player->AppendDirectionToObject(Trophy, PlayerState, false);
			PlayerState.Add(bEnemyCanMove ? 1.0f : 0.0f);

			float TrophyValue = GameOverlapResult->bGrabbedTrophy ? 1.0f : 0.0f;
			float KeyValue = GameOverlapResult->bGrabbedKey ? 1.0f : 0.0f;
			
			PlayerState.Add(TrophyValue);
			PlayerState.Add(KeyValue);
						
			float PlayerReward = 0.0f;

			bool bPlayerKilled = GameOverlapResult->bPlayerKilledByEnemy || GameOverlapResult->bPlayerKilledByTrap;			
			if (GameOverlapResult->bGameEnd && GameOverlapResult->bWonGame)
			{
				PlayerReward += GameOverlapResult->bGrabbedTrophy ? 10.0f : 2.0f;
			}
					
			if (GameOverlapResult->bGameEnd && !GameOverlapResult->bWonGame)
			{
				PlayerReward -= bPlayerKilled ? 4.0f : 2.0f;
			}

			UFPAgent* PlayerAgent = FIUBAPythonSubsystem.GetAgent("Player");	
			PlayerAction = PlayerAgent->Train(PlayerState, PlayerReward, GameOverlapResult->bGameEnd);
			bEndedForPlayer = GameOverlapResult->bGameEnd;
		}		
	
		if (!bEndedForPlayer)
		{
			int32 DeltaX = 0, DeltaY = 0;
			GetDeltaFromMovementAction(PlayerAction, DeltaX, DeltaY);
			GridTryMoveDeltaObject(Player, DeltaX, DeltaY);
		}
	}
	// Enemy! -----------------------
	
	if (!bEndedForEnemy && bEnemyCanMove && IsValid(Enemy))
	{
		int32 EnemyAction = 0;
		{
			TArray<float> EnemyState;
			AppendBorders(Enemy, EnemyState);
			Enemy->AppendDirectionToObject(Player, EnemyState, false);
			
			float EnemyReward = 0.0f;

			if (GameOverlapResult->bGameEnd && !GameOverlapResult->bPlayerKilledByEnemy)
			{
				EnemyReward -= 6.0f;
			}

			if (GameOverlapResult->bGameEnd && GameOverlapResult->bPlayerKilledByEnemy)
			{
				EnemyReward += 6.0f;
			}

			UFPAgent* EnemyAgent = FIUBAPythonSubsystem.GetAgent("Enemy");	
			EnemyAction = EnemyAgent->Train(EnemyState, EnemyReward, GameOverlapResult->bGameEnd);
			bEndedForEnemy = GameOverlapResult->bGameEnd;
		}

		if (!bEndedForEnemy)
		{
			int32 DeltaX = 0, DeltaY = 0;
			GetDeltaFromMovementAction(EnemyAction, DeltaX, DeltaY);
			GridTryMoveDeltaObject(Enemy, DeltaX, DeltaY);			
		}
	}
	
	bEnemyCanMove = !bEnemyCanMove;
	
	/*
	if (!bEndedForDoor)
	{
		int32 DoorAction = 0;
		{
			TArray<float> DoorState;
			AppendBorders(Door, DoorState);
			Player->AppendDirectionToObject(Door, DoorState);
			//Enemy->AppendDirectionToObject(Player, DoorState);
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
	*/
	
	if (bEndedForPlayer && (bEndedForEnemy || !IsValid(Enemy)))
		return true;

	return false;
}

void AFDLabyrinthGameMode::AppendBorders(AFDLabyrinthObject* Object, TArray<float>& OutBorders)
{
	int32 PlayerX, PlayerY;
	Object->GetGridLocation(PlayerX, PlayerY);

	int32 DirUp = PlayerX + 1;
	int32 DirDown = PlayerX - 1;
	int32 DirRight = PlayerY + 1;
	int32 DirLeft = PlayerY - 1;

	auto ExistObjectOrDanger = [this](int32 DirX, int32 DirY)
	{
		bool bBlocked = DirX >= GridSizeX || DirY >= GridSizeY || DirX < 0 || DirY < 0;
		return bBlocked || ExistObjectAt(DirX, DirY, EFDLabyrinthInteractiveObjectType::L_Trap);
	};
		
	OutBorders.Add(ExistObjectOrDanger(DirUp, PlayerY) ? 1 : 0);
	OutBorders.Add(ExistObjectOrDanger(DirDown, PlayerY) ? 1 : 0);
	OutBorders.Add(ExistObjectOrDanger(PlayerX, DirRight) ? 1 : 0);
	OutBorders.Add(ExistObjectOrDanger(PlayerX, DirLeft) ? 1 : 0);
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
		EFDLaberynthOverlapType OverlapType = DestinationObject->CanOverlap(Object);

		if (OverlapType == EFDLaberynthOverlapType::Blocked)
			return false;

		if (OverlapType == EFDLaberynthOverlapType::Overlapped)
		{
			GameOverlapResult->Clear();
			DestinationObject->OnOtherObjectTryToOverlap(Object, *GameOverlapResult.Get());
				
			if (GameOverlapResult->bDestroyObject)
			{
				PendingDestroy.Add(DestinationObject);
			}

			bEnemyCanMove = bEnemyCanMove && GameOverlapResult->bEnemyCanMove;
		}
	}

	while (!PendingDestroy.IsEmpty())
	{
		TWeakObjectPtr<AFDLabyrinthObject> PendingDestroyObject = PendingDestroy.Pop();
		DestroyObject(PendingDestroyObject.Get());
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

	AFDLabyrinthObject* NewActor = FindRecycledObject(ObjectClass);
	if (!IsValid(NewActor))
	{
		NewActor = GetWorld()->SpawnActor<AFDLabyrinthObject>(ObjectClass);
	}

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

const TArray<TWeakObjectPtr<AFDLabyrinthObject>>& AFDLabyrinthGameMode::GetObjectsAt(int32 X, int32 Y)
{
	const int32 GridIndex = GetGridIndex(X, Y);
	return GetLabyrinthObjects(GridIndex);
}

bool AFDLabyrinthGameMode::ExistObjectAt(int32 X, int32 Y, EFDLabyrinthInteractiveObjectType Type)
{
	const TArray<TWeakObjectPtr<AFDLabyrinthObject>>& ObjectsAtDestination = GetObjectsAt(X, Y);

	for (TWeakObjectPtr<AFDLabyrinthObject> Object : ObjectsAtDestination)
	{
		AFDLabyrinthInteractiveObject* InterObject = Cast<AFDLabyrinthInteractiveObject>(Object.Get());

		if (!IsValid(InterObject))
			continue;

		if (InterObject->GetInteractiveObjectType() == Type)
			return true;
	}

	return false;
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

void AFDLabyrinthGameMode::DestroyObject(AFDLabyrinthObject* Object)
{
	int32 PosX, PosY;
	Object->GetGridLocation(PosX, PosY);

	int32 GridIndex = GetGridIndex(PosX, PosY);
	
	LabyrinthGrid[GridIndex].Objects.Remove(Object);
	LabyrinthObjects.Remove(Object);

	CachedObjects.Add(Object);
	Object->MarkAsDestroyed();
}

AFDLabyrinthObject* AFDLabyrinthGameMode::FindRecycledObject(UClass* Class)
{
	AFDLabyrinthObject* RecycledObject = nullptr;
	for (auto Object : CachedObjects)
	{
		if (Object->IsA(Class))
		{
			RecycledObject = Object;
			break;
		}
	}

	if (IsValid(RecycledObject))
	{
		RecycledObject->OnCreated();
		CachedObjects.Remove(RecycledObject);
	}
	
	return RecycledObject;
}
