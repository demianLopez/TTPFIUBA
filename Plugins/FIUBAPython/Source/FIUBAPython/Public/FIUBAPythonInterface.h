// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FFIRewardValues
{
	float Coeficient = 0.0f;
	float Value = 0.0f;
};

class FIUBAPYTHON_API IFIUBAPythonInterface 
{
public:

	static IFIUBAPythonInterface& Get();
	virtual int32 InitializeTrain(const TArray<float>& Values, int32 ActionStateDim) = 0;
	virtual int32 Train(const TArray<float>& Values, const TArray<FFIRewardValues>& MaxValues, const TArray<FFIRewardValues>& MinValues, bool FinishTrain, bool FinishLoop) = 0;
};
