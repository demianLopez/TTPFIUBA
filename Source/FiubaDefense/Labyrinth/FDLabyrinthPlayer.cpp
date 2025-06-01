// Fill out your copyright notice in the Description page of Project Settings.


#include "Labyrinth/FDLabyrinthPlayer.h"

#include "FDLabyrinthInteractiveObject.h"


AFDLabyrinthPlayer::AFDLabyrinthPlayer()
{
}

void AFDLabyrinthPlayer::OnOtherObjectTryToOverlap(AFDLabyrinthObject* Overlap, FDOVerlapResult& Result)
{
	AFDLabyrinthInteractiveObject* OtherAsInteractive = Cast<AFDLabyrinthInteractiveObject>(Overlap);

	if (IsValid(OtherAsInteractive))
	{
		if (OtherAsInteractive->GetInteractiveObjectType() == EFDLabyrinthInteractiveObjectType::L_Enemy)
		{
			Result.bGameEnd = true;
			Result.bPlayerKilled = true;
		}
	}
}

bool AFDLabyrinthPlayer::CanOverlap(AFDLabyrinthObject* OtherObject)
{
	AFDLabyrinthInteractiveObject* OtherAsInteractive = Cast<AFDLabyrinthInteractiveObject>(OtherObject);

	if (IsValid(OtherAsInteractive))
	{
		return OtherAsInteractive->GetInteractiveObjectType() == EFDLabyrinthInteractiveObjectType::L_Enemy;
	}

	return false;
}
