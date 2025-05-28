// Fill out your copyright notice in the Description page of Project Settings.


#include "Labyrinth/FDLabyrinthInteractiveObject.h"

int32 AFDLabyrinthInteractiveObject::GetIdentifier() const
{
	int32 EnumAsInt = static_cast<int32>(InteractiveObjectType);
	return EnumAsInt;
}

void AFDLabyrinthInteractiveObject::OnOtherObjectTryToOverlap(AFDLabyrinthObject* Overlap, FDOVerlapResult& Result)
{

	switch (InteractiveObjectType)
	{
		case EFDLabyrinthInteractiveObjectType::L_Trap:
			Result.bGameEnd = true;
			Result.bWonGame = false;
			break;
		case EFDLabyrinthInteractiveObjectType::L_Trophy:
			Result.bGrabbedTrophy = true;
			Result.bDestroyObject = true;
			break;
		case EFDLabyrinthInteractiveObjectType::L_ExitDoor:
			Result.bGameEnd = true;
			Result.bWonGame = true;
	}
}

bool AFDLabyrinthInteractiveObject::CanOverlap(AFDLabyrinthObject* OtherObject)
{
	return true;
}
