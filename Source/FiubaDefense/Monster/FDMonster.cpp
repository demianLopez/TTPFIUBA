// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/FDMonster.h"

#include "GameFramework/FloatingPawnMovement.h"

// Sets default values
AFDMonster::AFDMonster()
{
	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnMovement"));
}

void AFDMonster::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	const FVector& Velocity = GetVelocity();
	SetActorRotation(Velocity.ToOrientationRotator());
}
