// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/BTTask_FindNearestTower.h"
#include "EngineUtils.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Tower/FDTower.h"

UBTTask_FindNearestTower::UBTTask_FindNearestTower()
{
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindNearestTower, BlackboardKey), AFDTower::StaticClass());
}

EBTNodeResult::Type UBTTask_FindNearestTower::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AController* ControllerOwner = OwnerComp.GetOwner<AController>();
	APawn* ControllingPawn = nullptr;
	if (ControllerOwner)
	{
		ControllingPawn = ControllerOwner->GetPawn();
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

	UBlackboardComponent* BBComponent = OwnerComp.GetBlackboardComponent();
	BBComponent->SetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID(), NearestTower);

	return EBTNodeResult::Succeeded;
}
