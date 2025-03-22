// Fill out your copyright notice in the Description page of Project Settings.

#include "FDTowerGridComponentTypes.h"

#include "FDTowerGridComponent.h"

FVector FFDTowerGridTile::GetTileWorldLocation() const
{
	FVector TowerLocation = OwningGridComponent->GetComponentLocation();
	FVector2D TileSize = OwningGridComponent->GetGridTileSize();
	
	FVector ThisTileRelativeLocation = FVector(TileLocation.X * TileSize.X, TileLocation.Y * TileSize.Y, 0.0f);

	return ThisTileRelativeLocation + TowerLocation;
}

FVector FFDTowerGridTile::GetTileWorldSize() const
{
	FVector2D TileSize = OwningGridComponent->GetGridTileSize();
	return FVector(TileSize.X, TileSize.Y, 0.0f);
}
