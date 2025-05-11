// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FDTowerGridComponent.generated.h"

struct FFDTowerGridTile;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIUBADEFENSE_API UFDTowerGridComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFDTowerGridComponent();

	virtual void BeginPlay() override;

	const TArray<TSharedPtr<FFDTowerGridTile>>& GetTowerGridTiles() const { return GridTiles; }
	const FVector2D& GetGridTileSize() const { return GridTileSize; }

	void GetBorderTiles(TArray<TSharedPtr<FFDTowerGridTile>>& OutBorderTiles);
	TSharedPtr<FFDTowerGridTile> GetTileAt(FIntVector2 GridTileLocation);

	const FIntVector2& GetGridSize() const { return GridSize; }

	FIntVector2 GetGridRadius() const { return FIntVector2(GridRadiusX, GridRadiusY); }
protected:

	void CreateGridTiles();

	UPROPERTY(EditAnywhere)
	int32 GridRadiusX = 5;
	
	UPROPERTY(EditAnywhere)
	int32 GridRadiusY = 5;

	FIntVector2 GridSize;

	UPROPERTY(EditAnywhere)
	FVector2D GridTileSize = FVector2D(100.0f);

	TArray<TSharedPtr<FFDTowerGridTile>> GridTiles;
};
