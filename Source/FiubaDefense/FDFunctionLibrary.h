// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FDFunctionLibrary.generated.h"

class AFDTower;
/**
 * 
 */
UCLASS()
class FIUBADEFENSE_API UFDFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static AFDTower* GetPlayerTower(const APlayerController* PlayerController);
};
