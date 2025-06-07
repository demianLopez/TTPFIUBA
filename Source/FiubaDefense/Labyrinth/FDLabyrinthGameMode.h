// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FDLabyrinthInteractiveObject.h"
#include "FDLabyrinthGameMode.generated.h"

class AFDLabyrinthInteractiveObject;
class AFDLabyrinthPlayer;
class AFDLabyrinthObject;

struct FLabyrinthGridData
{
	TArray<TWeakObjectPtr<AFDLabyrinthObject>> Objects;
	
	int32 X;
	int32 Y;
};

/**
 * 
 */
UCLASS()
class FIUBADEFENSE_API AFDLabyrinthGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void StartPlay() override;

protected:

	void PlayMultipleMatches();
	void RestartMatch();

	UPROPERTY(EditAnywhere)
	int32 GridSizeX = 7;

	UPROPERTY(EditAnywhere)
	int32 GridSizeY = 7;

	UPROPERTY(EditAnywhere)
	float TileDimension = 100.0f;

	TArray<FLabyrinthGridData> LabyrinthGrid;
	TArray<TWeakObjectPtr<AFDLabyrinthObject>> LabyrinthObjects;

	int32 GetGridIndex(int32 X, int32 Y);
	void GetGridCoordinate(int32 Index, int32& OutX, int32& OutY);
	void AppendBorders(AFDLabyrinthObject* Object, TArray<float>& OutBorders);
	
	const TArray<TWeakObjectPtr<AFDLabyrinthObject>>& GetLabyrinthObjects(int32 Index);
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AFDLabyrinthObject>> RandomObjects;

	FVector GetTileWorldLocation(int32 X, int32 Y);

	AFDLabyrinthObject* SpawnObjectRandomly(TSubclassOf<AFDLabyrinthObject> ObjectClass, FRandomStream& RandomStream);
	AFDLabyrinthObject* SpawnObjectAt(TSubclassOf<AFDLabyrinthObject> ObjectClass, int32 X, int32 Y, bool bSpawnAlone = false);

	bool GridTryMoveDeltaObject(AFDLabyrinthObject* Object, int32 DeltaX, int32 DeltaY);
	bool GridTryMoveObject(AFDLabyrinthObject* Object, int32 DestX, int32 DestY);
	
	const TArray<TWeakObjectPtr<AFDLabyrinthObject>>& GetObjectsAt(int32 X, int32 Y);
	bool ExistObjectAt(int32 X, int32 Y, EFDLabyrinthInteractiveObjectType Type);

	void DrawGrid();

	void AdvanceTurn();
	bool Internal_AdvanceTurn();
	
	template<class T>
	T* FindObject()
	{
		return Cast<T>(FindObject(T::StaticClass()));
	}

	AFDLabyrinthInteractiveObject* FindInteractiveObject(EFDLabyrinthInteractiveObjectType Type);
	
	AFDLabyrinthObject* FindObject(TSubclassOf<AFDLabyrinthObject> ObjectClass);
	
	int32 TurnNumber = 0;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxTurns = 40;
		
	friend class AFDLabyrinthObject;
	
	bool bEndedForPlayer = false;
	bool bEndedForEnemy = false;
	bool bEndedForDoor = false;

	TSharedPtr<struct FDOVerlapResult> GameOverlapResult;
			
	float LastDistSquared = 0.0f;

	void GetDeltaFromMovementAction(int32 Action, int32& OutDeltaX, int32& OutDeltaY);
	void AppendBinaryToFloatArray(int32 Number, int32 NumDigits, TArray<float>& OutArray);

	static int32 TotalEnemyWon;
	static int32 TotalPlayerWon;

	// ---------- Cached

	void DestroyObject(AFDLabyrinthObject* Object);
	
	UPROPERTY(Transient)
	TArray<AFDLabyrinthObject*> CachedObjects;

	AFDLabyrinthObject* FindRecycledObject(UClass* Class);
};
