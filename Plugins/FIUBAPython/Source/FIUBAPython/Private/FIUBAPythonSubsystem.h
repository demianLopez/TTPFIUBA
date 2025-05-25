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

	virtual int32 InitializeTrain(const TArray<float>& Values, int32 ActionStateDim) override;

	virtual int32 Train(const TArray<float>& Values, float Reward, bool FinishTrain, bool FinishLoop) override;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual float GetTimeBetweenRounds() const override { return TimeBetweenRounds;};

protected:

	void InitPython();
	void DeinitPython();

	void StartAutonomousTraining(int32 NumberOfRounds);
	void EndAutonomousTraining();
	bool IsPerformingAutonomousTraining() const;

	void EndCurrentMatch();
	void StartNextMatch();
	
	bool bPythonInitialized = false;
	bool bTrainInitialized = false;

	FFPyObjectPtr FrameworkPythonObject;
	PyMethodDef* callbackDef;

	TSharedPtr<class FPythonSubsystemExec> SubsystemExec;

	UFUNCTION(Exec)
	void ExecuteTraining(int32 NumberOfMatches, float TimeBetweenRounds);

#if WITH_EDITOR
	void StartPlayInEditor();
	void EndPlayInEditor();

	void OnShutdownPIE(bool bSimulating);
#endif
	
	int32 RemainingTrainings = 0;
	bool bPerformingAutonomousTraining = false;
	float TimeBetweenRounds = 0.0f;
};
