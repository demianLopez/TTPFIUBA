// Fill out your copyright notice in the Description page of Project Settings.

#include "Labyrinth/FDLabyrinthPlayer.h"

#include "FDLabyrinthEnemy.h"

AFDLabyrinthPlayer::AFDLabyrinthPlayer()
{
	InteractiveObjectType = EFDLabyrinthInteractiveObjectType::L_Player;
}

EFDLaberynthOverlapType AFDLabyrinthPlayer::CanOverlap(AFDLabyrinthObject* OtherObject)
{
	AFDLabyrinthEnemy* OtherAsEnemy = Cast<AFDLabyrinthEnemy>(OtherObject);

	if (!IsValid(OtherAsEnemy))
		return EFDLaberynthOverlapType::Blocked;

	return EFDLaberynthOverlapType::Overlapped;
}

void AFDLabyrinthPlayer::OnOtherObjectTryToOverlap(AFDLabyrinthObject* Overlap, FDOVerlapResult& Result)
{
	Result.bGameEnd = true;
	Result.bPlayerKilled = true;	
}