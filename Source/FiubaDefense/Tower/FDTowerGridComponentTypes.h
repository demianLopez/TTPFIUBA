// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class AFDMonster;
class UFDTowerGridComponent;

struct FIUBADEFENSE_API FFDTowerGridTile
{
	FIntVector2 TileLocation;

	TWeakObjectPtr<UFDTowerGridComponent> OwningGridComponent;

	TArray<TWeakObjectPtr<AFDMonster>> MonstersInTile;

	FVector GetTileWorldLocation() const;
	FVector GetTileWorldSize() const;

	bool bIsBorderTile = false;
};

