// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower/FDTowerGridComponent.h"

#include "FDTowerGridComponentTypes.h"

UFDTowerGridComponent::UFDTowerGridComponent()
{

}

void UFDTowerGridComponent::BeginPlay()
{
	Super::BeginPlay();

	CreateGridTiles();
}

void UFDTowerGridComponent::GetBorderTiles(TArray<TSharedPtr<FFDTowerGridTile>>& OutBorderTiles)
{
	for (TSharedPtr<FFDTowerGridTile> GridTile : GridTiles)
	{
		if (GridTile->bIsBorderTile)
		{
			OutBorderTiles.Add(GridTile);
		}
	}
}

TSharedPtr<FFDTowerGridTile> UFDTowerGridComponent::GetTileAt(FIntVector2 GridTileLocation)
{
	for (TSharedPtr<FFDTowerGridTile> GridTile : GridTiles)
	{
		if (GridTile->TileLocation == GridTileLocation)
		{
			return GridTile;
		}
	}

	return nullptr;
}

void UFDTowerGridComponent::CreateGridTiles()
{
	GridSize.X = GridRadiusX * 2 + 1;
	GridSize.Y = GridRadiusY * 2 + 1;
	
	for (int32 X = -GridRadiusX; X <= GridRadiusX; X++)
	{
		for (int32 Y = -GridRadiusY; Y <= GridRadiusY; Y++)
		{
			if (X == 0 && Y == 0)
				continue;

			TSharedPtr<FFDTowerGridTile> NewTowerGridTile = MakeShared<FFDTowerGridTile>();
			NewTowerGridTile->TileLocation = FIntVector2(X, Y);
			NewTowerGridTile->OwningGridComponent = this;

			bool bIsBorder = FMath::Abs(X) == GridRadiusX || FMath::Abs(Y) == GridRadiusY;
			NewTowerGridTile->bIsBorderTile = bIsBorder;

			GridTiles.Add(NewTowerGridTile);			
		}
	}
}
