// Fill out your copyright notice in the Description page of Project Settings.


#include "Labyrinth/FDLabyrinthEnemy.h"

#include "FDLabyrinthPlayer.h"

AFDLabyrinthEnemy::AFDLabyrinthEnemy()
{
	InteractiveObjectType = EFDLabyrinthInteractiveObjectType::L_Enemy;
}

EFDLaberynthOverlapType AFDLabyrinthEnemy::CanOverlap(AFDLabyrinthObject* OtherObject)
{
	AFDLabyrinthPlayer* OtherObjectAsPlayer = Cast<AFDLabyrinthPlayer>(OtherObject);

	if (!IsValid(OtherObjectAsPlayer))
		return EFDLaberynthOverlapType::Ignored;

	return EFDLaberynthOverlapType::Overlapped;
}

void AFDLabyrinthEnemy::OnOtherObjectTryToOverlap(AFDLabyrinthObject* Overlap, FDOVerlapResult& Result)
{
	Result.bGameEnd = true;
	Result.bPlayerKilled = true;
}