// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "FPSubsystem.generated.h"

class FJsonObject;

/**
 * 
 */
UCLASS()
class FIUBAPYTHON_API UFPSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:

	static UFPSubsystem* Get();

	void GetResponse();

protected:

	TSharedPtr<FJsonObject> GetBody();
	
};
