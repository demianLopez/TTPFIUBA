// Fill out your copyright notice in the Description page of Project Settings.


#include "Labyrinth/FDLabyrinthTrap.h"

#include "FDLabyrinthEnemy.h"
#include "FDLabyrinthPlayer.h"

AFDLabyrinthTrap::AFDLabyrinthTrap()
{
	InteractiveObjectType = EFDLabyrinthInteractiveObjectType::L_Trap;
}

EFDLaberynthOverlapType AFDLabyrinthTrap::CanOverlap(AFDLabyrinthObject* OtherObject)
{
	AFDLabyrinthPlayer* OtherObjectAsPlayer = Cast<AFDLabyrinthPlayer>(OtherObject);
	AFDLabyrinthEnemy* OtherAsEnemy = Cast<AFDLabyrinthEnemy>(OtherObject);
	
	if (!IsValid(OtherObjectAsPlayer) && !IsValid(OtherAsEnemy))
		return EFDLaberynthOverlapType::Ignored;

	return EFDLaberynthOverlapType::Overlapped;
}

void AFDLabyrinthTrap::OnOtherObjectTryToOverlap(AFDLabyrinthObject* Overlap, FDOVerlapResult& Result)
{
	AFDLabyrinthPlayer* OtherObjectAsPlayer = Cast<AFDLabyrinthPlayer>(Overlap);

	if (IsValid(OtherObjectAsPlayer))
	{
		Result.bGameEnd = true;
		Result.bPlayerKilledByTrap = true;
	}

	Result.bEnemyCanMove = false;
}
