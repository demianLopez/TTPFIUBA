// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FIUBAPythonInterface.h"
#include "Subsystems/EngineSubsystem.h"
#include "FIUBAPythonTypes.h"
#include "FIUBAPythonSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class UFIUBAPythonSubsystem : public UEngineSubsystem, public IFIUBAPythonInterface
{
	GENERATED_BODY()

public:

	static UFIUBAPythonSubsystem& Get();

	virtual void InitModel() override;
	virtual void Train() override;

protected:

	void InitPython();
	void DeinitPython();
	
	bool bPythonInitialized = false;

	FFPyObjectPtr FrameworkPythonObject;
};
