// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class FIUBAPYTHON_API IFIUBAPythonInterface
{
public:

	static IFIUBAPythonInterface& Get();
	virtual void InitModel() = 0;
	virtual void Train() = 0;
};
