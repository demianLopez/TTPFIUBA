// Fill out your copyright notice in the Description page of Project Settings.


#include "Labyrinth/FDLabyrinthTrophy.h"

#include "FDLabyrinthPlayer.h"

AFDLabyrinthTrophy::AFDLabyrinthTrophy()
{
	InteractiveObjectType = EFDLabyrinthInteractiveObjectType::L_Trophy;
}

EFDLaberynthOverlapType AFDLabyrinthTrophy::CanOverlap(AFDLabyrinthObject* OtherObject)
{
	AFDLabyrinthPlayer* OtherObjectAsPlayer = Cast<AFDLabyrinthPlayer>(OtherObject);

	if (!IsValid(OtherObjectAsPlayer))
		return EFDLaberynthOverlapType::Ignored;

	return EFDLaberynthOverlapType::Overlapped;
}

void AFDLabyrinthTrophy::OnOtherObjectTryToOverlap(AFDLabyrinthObject* Overlap, FDOVerlapResult& Result)
{
	Super::OnOtherObjectTryToOverlap(Overlap, Result);

	Result.bGrabbedTrophy = true;
	Result.bDestroyObject = true;
}


