// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FDPlayerState.h"

void AFDPlayerState::IncrementIncome()
{
	Gold += 500.0f;
}

void AFDPlayerState::SubstractGold(float Amount)
{
	Gold = FMath::Max(0.0f, Gold - Amount);
}

void AFDPlayerState::AddGold(float GoldAmount)
{
	Gold += GoldAmount;
}

void AFDPlayerState::IncrementKillCount()
{
	EnemiesKilled++;
}
