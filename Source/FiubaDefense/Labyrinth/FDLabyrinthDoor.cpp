// Fill out your copyright notice in the Description page of Project Settings.


#include "Labyrinth/FDLabyrinthDoor.h"

#include "FDLabyrinthPlayer.h"

AFDLabyrinthDoor::AFDLabyrinthDoor()
{
	InteractiveObjectType = EFDLabyrinthInteractiveObjectType::L_ExitDoor;
}

EFDLaberynthOverlapType AFDLabyrinthDoor::CanOverlap(AFDLabyrinthObject* OtherObject)
{
	AFDLabyrinthPlayer* OtherObjectAsPlayer = Cast<AFDLabyrinthPlayer>(OtherObject);

	if (!IsValid(OtherObjectAsPlayer))
		return EFDLaberynthOverlapType::Blocked;

	return EFDLaberynthOverlapType::Overlapped;
}

void AFDLabyrinthDoor::OnOtherObjectTryToOverlap(AFDLabyrinthObject* Overlap, FDOVerlapResult& Result)
{
	Result.bGameEnd = true;
	Result.bWonGame = true;
}


