// Fill out your copyright notice in the Description page of Project Settings.


#include "Labyrinth/FDLabyrinthTrap.h"

#include "FDLabyrinthPlayer.h"

AFDLabyrinthTrap::AFDLabyrinthTrap()
{
	InteractiveObjectType = EFDLabyrinthInteractiveObjectType::L_Trap;
}

EFDLaberynthOverlapType AFDLabyrinthTrap::CanOverlap(AFDLabyrinthObject* OtherObject)
{
	AFDLabyrinthPlayer* OtherObjectAsPlayer = Cast<AFDLabyrinthPlayer>(OtherObject);

	if (!IsValid(OtherObjectAsPlayer))
		return EFDLaberynthOverlapType::Ignored;

	return EFDLaberynthOverlapType::Overlapped;
}

void AFDLabyrinthTrap::OnOtherObjectTryToOverlap(AFDLabyrinthObject* Overlap, FDOVerlapResult& Result)
{
	Result.bGameEnd = true;
	Result.bPlayerKilled = true;
}
