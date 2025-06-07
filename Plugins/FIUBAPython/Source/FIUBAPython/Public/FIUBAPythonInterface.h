// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FFIRewardValues
{
	FFIRewardValues() : Coeficient(1.0f), Value(0.0f) {}
	float Coeficient;
	float Value;
};

class UFPAgent;

class FIUBAPYTHON_API IFIUBAPythonInterface 
{
public:

	static IFIUBAPythonInterface& Get();

	virtual UFPAgent* CreateAgent(const FString& AgentName, int32 SateDim, int32 ActionStateDim) = 0;
	virtual UFPAgent* GetAgent(const FString& AgentName) = 0;
	virtual void InitEpisode() = 0;

	virtual float GetTimeBetweenRounds() const = 0;
	virtual bool IsFastRun() const = 0;
};
