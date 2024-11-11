// Fill out your copyright notice in the Description page of Project Settings.


#include "FDFunctionLibrary.h"

#include "Player/FDPlayerPawn.h"

AFDTower* UFDFunctionLibrary::GetPlayerTower(const APlayerController* PlayerController)
{
	AFDPlayerPawn* PlayerPawn = PlayerController->GetPawn<AFDPlayerPawn>();

	if(IsValid(PlayerPawn))
	{
		return PlayerPawn->GetTowerReference();
	}

	return nullptr;
}
