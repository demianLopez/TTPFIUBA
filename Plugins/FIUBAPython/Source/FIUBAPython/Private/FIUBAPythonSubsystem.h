// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FIUBAPythonInterface.h"
#include "Subsystems/EngineSubsystem.h"
#include "FIUBAPythonTypes.h"
#include "pytypedefs.h"
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
	
	virtual UFPAgent* CreateAgent(const FString& AgentName, int32 SateDim, int32 ActionStateDim) override;
	virtual UFPAgent* GetAgent(const FString& AgentName) override;
	
	virtual void InitEpisode() override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual bool IsPerformingAutonomousTraining() const override;
	virtual bool HasPendingMatch() override;

protected:

	void OnAgentEpisodeFinished();

	void InitPython();
	void DeinitPython();
	
	void EndAutonomousTraining();

	void EndCurrentMatch();
	void StartNextMatch();
	
	bool bPythonInitialized = false;

	FFPyObjectPtr FrameworkPythonObject;
	PyMethodDef* callbackDef;

	TSharedPtr<class FPythonSubsystemExec> SubsystemExec;

	UFUNCTION(Exec)
	void ExecuteTraining(int32 NumberOfMatches);
	
#if WITH_EDITOR
	void StartPlayInEditor();
	void EndPlayInEditor();
	void OnShutdownPIE(bool bSimulating);
#endif
	
	int32 RemainingTrainings = 0;
	bool bPerformingAutonomousTraining = false;

	UPROPERTY(Transient)
	TMap<FString, TObjectPtr<UFPAgent>> Agents;
};
