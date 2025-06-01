// Fill out your copyright notice in the Description page of Project Settings.


#include "Labyrinth/FDLabyrinthInteractiveObject.h"

int32 AFDLabyrinthInteractiveObject::GetIdentifier() const
{
	int32 EnumAsInt = static_cast<int32>(InteractiveObjectType);
	return EnumAsInt;
}

void AFDLabyrinthInteractiveObject::OnOtherObjectTryToOverlap(AFDLabyrinthObject* Overlap, FDOVerlapResult& Result)
{
	if (Overlap == this)
		return;

	bool bImEnemy = InteractiveObjectType == EFDLabyrinthInteractiveObjectType::L_Enemy;
	
	AFDLabyrinthInteractiveObject* OtherAsInteractive = Cast<AFDLabyrinthInteractiveObject>(Overlap);
	if (IsValid(OtherAsInteractive))
	{
		bool bOtherIsEnemy = OtherAsInteractive->InteractiveObjectType == EFDLabyrinthInteractiveObjectType::L_Enemy;
		bool bOtherIsDoor = OtherAsInteractive->InteractiveObjectType == EFDLabyrinthInteractiveObjectType::L_ExitDoor;
		
		if (bOtherIsDoor && bImEnemy)
			return;

		if (bOtherIsEnemy)
			return;
	}
	
	switch (InteractiveObjectType)
	{
		case EFDLabyrinthInteractiveObjectType::L_Enemy:
			Result.bGameEnd = true;
			Result.bWonGame = false;
			Result.bPlayerKilled = true;
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
	AFDLabyrinthInteractiveObject* OtherAsInteractive = Cast<AFDLabyrinthInteractiveObject>(OtherObject);

	if (IsValid(OtherAsInteractive))
	{
		bool bImDoor = InteractiveObjectType == EFDLabyrinthInteractiveObjectType::L_ExitDoor;
		bool bImTrophy = InteractiveObjectType == EFDLabyrinthInteractiveObjectType::L_Trophy;
		bool bImEnemy = InteractiveObjectType == EFDLabyrinthInteractiveObjectType::L_Enemy;
		
		bool bOtherIsEnemy = OtherAsInteractive->InteractiveObjectType == EFDLabyrinthInteractiveObjectType::L_Enemy;
		bool bOtherIsDoor = OtherAsInteractive->InteractiveObjectType == EFDLabyrinthInteractiveObjectType::L_ExitDoor;

		if (bImEnemy && bOtherIsDoor)
			return false;
		
		if ((bImTrophy || bImDoor) && bOtherIsEnemy)
			return false;
	}
	
	return true;
}
