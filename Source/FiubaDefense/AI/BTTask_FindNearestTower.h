// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_FindNearestTower.generated.h"

/**
 * 
 */
UCLASS()
class FIUBADEFENSE_API UBTTask_FindNearestTower : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_FindNearestTower();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
