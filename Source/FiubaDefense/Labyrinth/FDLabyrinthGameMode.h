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

	bool IsChannelOccupied(int32 Channel);

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

	const TArray<TWeakObjectPtr<AFDLabyrinthObject>>& GetLabyrinthObjects(int32 Index);
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AFDLabyrinthObject>> RandomObjects;

	FVector GetTileWorldLocation(int32 X, int32 Y);

	AFDLabyrinthObject* SpawnObjectRandomly(TSubclassOf<AFDLabyrinthObject> ObjectClass, FRandomStream& RandomStream);
	AFDLabyrinthObject* SpawnObjectAt(TSubclassOf<AFDLabyrinthObject> ObjectClass, int32 X, int32 Y, bool bSpawnAlone = false);

	bool GridTryMoveObject(AFDLabyrinthObject* Object, int32 DestX, int32 DestY);
	const TArray<TWeakObjectPtr<AFDLabyrinthObject>>& GetObjectsAt(int32 X, int32 Y);
	void DrawGrid();

	void AdvanceTurn();

	TWeakObjectPtr<AFDLabyrinthPlayer> CachedPlayer;
	AFDLabyrinthPlayer* FindPlayer();

	template<class T>
	T* FindObject()
	{
		return Cast<T>(FindObject(T::StaticClass()));
	}

	AFDLabyrinthInteractiveObject* FindInteractiveObject(EFDLabyrinthInteractiveObjectType Type);
	
	AFDLabyrinthObject* FindObject(TSubclassOf<AFDLabyrinthObject> ObjectClass);
	
	int32 TurnNumber = 0;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxTurns = 25;

	void ObjectRemoved(AFDLabyrinthObject* Object);
	
	friend class AFDLabyrinthObject;

	bool bGameEnded = false;
	bool bWonGame = false;
	bool bMoved = false;

	bool bGrabbedTrophy = false;
	
	float LastDistSquared = 0.0f;
};
