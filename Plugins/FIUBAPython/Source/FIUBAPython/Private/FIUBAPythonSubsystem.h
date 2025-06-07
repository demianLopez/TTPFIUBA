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
	virtual float GetTimeBetweenRounds() const override { return TimeBetweenRounds;};

	virtual bool IsFastRun() const override;

protected:

	void OnAgentEpisodeFinished();

	void InitPython();
	void DeinitPython();

	void StartAutonomousTraining(int32 NumberOfRounds);
	void EndAutonomousTraining();
	bool IsPerformingAutonomousTraining() const;

	void EndCurrentMatch();
	void StartNextMatch();
	
	bool bPythonInitialized = false;

	FFPyObjectPtr FrameworkPythonObject;
	PyMethodDef* callbackDef;

	TSharedPtr<class FPythonSubsystemExec> SubsystemExec;

	UFUNCTION(Exec)
	void ExecuteTraining(int32 NumberOfMatches, float TimeBetweenRounds, int32 RunBetweenTicks = 100);

	UFUNCTION(Exec)
	void DoFastTraining(int32 NumberOfMatches);
	
#if WITH_EDITOR
	void StartPlayInEditor();
	void EndPlayInEditor();

	void OnShutdownPIE(bool bSimulating);
#endif
	
	int32 RemainingTrainings = 0;
	bool bPerformingAutonomousTraining = false;
	float TimeBetweenRounds = 0.0f;
	int32 RunsTillSlowRun = 0;

	bool bFastTraining = false;

	UPROPERTY(Transient)
	TMap<FString, TObjectPtr<UFPAgent>> Agents;
};
