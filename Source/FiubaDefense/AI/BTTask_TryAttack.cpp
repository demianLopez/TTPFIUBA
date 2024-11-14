// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/BTTask_TryAttack.h"

#include "EngineUtils.h"
#include "Monster/FDMonster.h"
#include "Tower/FDTower.h"

EBTNodeResult::Type UBTTask_TryAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AController* ControllerOwner = OwnerComp.GetOwner<AController>();
	AFDMonster* ControllingPawn = nullptr;
	if (ControllerOwner)
	{
		ControllingPawn = ControllerOwner->GetPawn<AFDMonster>();
	}

	if(!IsValid(ControllingPawn))
		return EBTNodeResult::Failed;
	
	UWorld* World = GetWorld();

	AFDTower* NearestTower = nullptr;
	float NearestDistanceSQRT = 0.0f;

	for(TActorIterator<AFDTower> It(World) ; It; ++It)
	{
		AFDTower* Tower = *It;

		const float DistanceToTowerSQRT = Tower->GetSquaredHorizontalDistanceTo(ControllingPawn);

		if(!IsValid(NearestTower) || DistanceToTowerSQRT < NearestDistanceSQRT)
		{
			NearestDistanceSQRT = DistanceToTowerSQRT;
			NearestTower = Tower;
		}
	}

	if(!IsValid(NearestTower))
		return EBTNodeResult::Failed;

	if(ControllingPawn->TryAttack(NearestTower))
	{
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
