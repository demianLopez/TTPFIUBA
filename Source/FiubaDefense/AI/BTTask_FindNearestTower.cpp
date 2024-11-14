// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/BTTask_FindNearestTower.h"
#include "EngineUtils.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Monster/FDMonster.h"
#include "Tower/FDTower.h"

UBTTask_FindNearestTower::UBTTask_FindNearestTower()
{
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindNearestTower, BlackboardKey));
}

EBTNodeResult::Type UBTTask_FindNearestTower::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	FVector GroundPoint = NearestTower->GetFeetLocation();

	FVector SegmentToMonster = ControllingPawn->GetActorLocation() - GroundPoint;
	SegmentToMonster.Z = 0.0f;

	GroundPoint += SegmentToMonster.GetSafeNormal() * ControllingPawn->GetAttackRange();
	
	//DrawDebugSphere(GetWorld(), GroundPoint, 50.0f, 8, FColor::Red, false, 2.0f);
	
	UBlackboardComponent* BBComponent = OwnerComp.GetBlackboardComponent();
	BBComponent->SetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID(), GroundPoint);

	return EBTNodeResult::Succeeded;
}
