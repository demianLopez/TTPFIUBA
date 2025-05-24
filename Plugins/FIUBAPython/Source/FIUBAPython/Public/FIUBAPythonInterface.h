// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FFIRewardValues
{
	FFIRewardValues() : Coeficient(1.0f), Value(0.0f) {}
	float Coeficient;
	float Value;
};

class FIUBAPYTHON_API IFIUBAPythonInterface 
{
public:

	static IFIUBAPythonInterface& Get();
	virtual int32 InitializeTrain(const TArray<float>& Values, int32 ActionStateDim) = 0;
	virtual int32 Train(const TArray<float>& Values, const TArray<FFIRewardValues>& MaxValues, const TArray<FFIRewardValues>& MinValues, bool FinishTrain, bool FinishLoop) = 0;
	virtual float GetTimeBetweenRounds() const = 0;
};
