// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FPAgent.generated.h"

struct FFPObjectContainer;

DECLARE_LOG_CATEGORY_EXTERN(FDPythonLog, Log, All);

/**
 * 
 */
UCLASS()
class FIUBAPYTHON_API UFPAgent : public UObject
{
	GENERATED_BODY()

public:

	DECLARE_MULTICAST_DELEGATE(FAgentDelegate);
	FAgentDelegate OnEpisodeFinished;
	
	int32 Train(const TArray<float>& State, float Reward, bool EpisodeFinished);

	bool IsEpisodeFinished() const { return bEpisodeFinished; }
protected:

	void EpisodeStart();
	
	void InitAgent(const FString& AgentName, int32 SateDim, int32 ActionStateDim);

	friend class UFIUBAPythonSubsystem;

	FString AgentName;

	TSharedPtr<FFPObjectContainer> Container;
	struct PyMethodDef* callbackDef;

	bool bTrainInitialized = false;
	int32 StateDim = 0;

	bool bEpisodeFinished = false;;
};
