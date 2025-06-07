// Fill out your copyright notice in the Description page of Project Settings.

#include "FIUBAPythonSubsystem.h"

#include "FPAgent.h"
#include "IAssetViewport.h"
#include "LevelEditor.h"
#include "Python.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"

class FPythonSubsystemExec : private FSelfRegisteringExec
{
public:
		
	FPythonSubsystemExec(UFIUBAPythonSubsystem* InWeatherSubsystem) : FSelfRegisteringExec()
	{
		PythonSubsystem = InWeatherSubsystem;
	}
	
	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override
	{
		if (!PythonSubsystem.IsValid())
			return false;
		
		return PythonSubsystem->ProcessConsoleExec(Cmd, Ar, InWorld);
	}

protected:

	TWeakObjectPtr<UFIUBAPythonSubsystem> PythonSubsystem;
};

UFIUBAPythonSubsystem& UFIUBAPythonSubsystem::Get()
{
	return *GEngine->GetEngineSubsystem<UFIUBAPythonSubsystem>();
}

void UFIUBAPythonSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	SubsystemExec = MakeShareable(new FPythonSubsystemExec(this));
}

void UFIUBAPythonSubsystem::ExecuteTraining(int32 NumberOfMatches, float InTimeBetweenRounds, int32 RunBetweenTicks)
{
	TimeBetweenRounds = InTimeBetweenRounds;
	RunsTillSlowRun = RunBetweenTicks;
	StartAutonomousTraining(NumberOfMatches);
}

void UFIUBAPythonSubsystem::DoFastTraining(int32 NumberOfMatches)
{
	if (bPerformingAutonomousTraining)
		return;

	bFastTraining = true;
	RemainingTrainings = NumberOfMatches;
	bPerformingAutonomousTraining = true;
	
	StartPlayInEditor();
}

bool UFIUBAPythonSubsystem::IsPerformingAutonomousTraining() const
{
	return bPerformingAutonomousTraining;
}

void UFIUBAPythonSubsystem::StartAutonomousTraining(int32 NumberOfRounds)
{
	if (bPerformingAutonomousTraining)
		return;
	
	RemainingTrainings = NumberOfRounds;
	bPerformingAutonomousTraining = true;
	
#if WITH_EDITOR
	FEditorDelegates::ShutdownPIE.AddUObject(this, &UFIUBAPythonSubsystem::OnShutdownPIE);
	StartPlayInEditor();	
#endif
}

void UFIUBAPythonSubsystem::EndAutonomousTraining()
{
	if (!bPerformingAutonomousTraining)
		return;

#if WITH_EDITOR
	FEditorDelegates::ShutdownPIE.RemoveAll(this);
#endif
	
	bPerformingAutonomousTraining = false;
	RemainingTrainings = 0;
}

bool UFIUBAPythonSubsystem::IsFastRun() const
{
	if (RunsTillSlowRun == 0)
		return false;
	
	return (RemainingTrainings % RunsTillSlowRun) != 0;
}

UFPAgent* UFIUBAPythonSubsystem::CreateAgent(const FString& AgentName, int32 SateDim, int32 ActionStateDim)
{
	if (!ensure(bPythonInitialized))
	{
		return nullptr;
	}

	TObjectPtr<UFPAgent>& Agent = Agents.FindOrAdd(AgentName);

	if (IsValid(Agent))
	{
		return Agent.Get();
	}

	Agent = NewObject<UFPAgent>(this, UFPAgent::StaticClass(), *AgentName, RF_Transient);
	Agent->InitAgent(AgentName, SateDim, ActionStateDim);
	Agent->OnEpisodeFinished.AddUObject(this, &ThisClass::OnAgentEpisodeFinished);
	
	return Agent.Get();
}

UFPAgent* UFIUBAPythonSubsystem::GetAgent(const FString& AgentName)
{
	if (Agents.Contains(AgentName))
	{
		return Agents[AgentName].Get();
	}

	return nullptr;
}

void UFIUBAPythonSubsystem::InitEpisode()
{
	if (!bPythonInitialized)
	{
		InitPython();
	}

	TArray<FString> AgentNames;
	Agents.GetKeys(AgentNames);

	for (const FString& AgentName : AgentNames)
	{
		Agents[AgentName]->EpisodeStart();
	}
}

void UFIUBAPythonSubsystem::InitPython()
{
	if (bPythonInitialized)
		return;

	bPythonInitialized = true;

	Py_Initialize();

	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.insert(0, 'D:/FIUBASERVER/fiubaenv/Lib/site-packages')");

	PyRun_SimpleString("import os");
	PyRun_SimpleString("os.chdir('D:/FIUBASERVER/main')");
	
	FString ImportString = "sys.path.append('D:/FIUBASERVER/main')";
	PyRun_SimpleString(TCHAR_TO_UTF8(*ImportString));
}

void UFIUBAPythonSubsystem::DeinitPython()
{
	if (!bPythonInitialized)
		return;

	bPythonInitialized = false;
	
	FrameworkPythonObject.Clear();
	
	Py_Finalize();
}

void UFIUBAPythonSubsystem::OnAgentEpisodeFinished()
{
	TArray<FString> AgentNames;
	Agents.GetKeys(AgentNames);

	for (const FString& AgentName : AgentNames)
	{
		if (!Agents[AgentName]->IsEpisodeFinished())
		{
			return;
		}
	}
	
	EndCurrentMatch();
}

void UFIUBAPythonSubsystem::EndCurrentMatch()
{
	if (!IsPerformingAutonomousTraining())
		return;

	if (bFastTraining)
	{
		RemainingTrainings--;
	}
	
#if WITH_EDITOR
	EndPlayInEditor();
#endif
}

void UFIUBAPythonSubsystem::StartNextMatch()
{
#if WITH_EDITOR
	StartPlayInEditor();
#endif
}

#if WITH_EDITOR

void UFIUBAPythonSubsystem::OnShutdownPIE(bool bSimulating)
{
	if (!IsPerformingAutonomousTraining())
		return;

	if (RemainingTrainings <= 0)
	{
		EndAutonomousTraining();
		return;
	}

	RemainingTrainings--;
	StartPlayInEditor();
}

void UFIUBAPythonSubsystem::StartPlayInEditor()
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
		
	ULevelEditorPlaySettings* PlaySettings = GetMutableDefault<ULevelEditorPlaySettings>();
	PlaySettings->LastExecutedPlayModeType = PlayMode_InViewPort;

	FPropertyChangedEvent PropChangeEvent(ULevelEditorPlaySettings::StaticClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(ULevelEditorPlaySettings, LastExecutedPlayModeType)));
	PlaySettings->PostEditChangeProperty(PropChangeEvent);

	PlaySettings->SaveConfig();

	TSharedPtr<IAssetViewport> ActiveLevelViewport = LevelEditorModule.GetFirstActiveViewport();
	
	FRequestPlaySessionParams SessionParams;

	// Make sure we can find a path to the view port.  This will fail in cases where the view port widget
	// is in a backgrounded tab, etc.  We can't currently support starting PIE in a backgrounded tab
	// due to how PIE manages focus and requires event forwarding from the application.
	if (ActiveLevelViewport.IsValid() && FSlateApplication::Get().FindWidgetWindow(ActiveLevelViewport->AsWidget()).IsValid())
	{
		SessionParams.DestinationSlateViewport = ActiveLevelViewport;
	}
			
	GUnrealEd->RequestPlaySession(SessionParams);
}

void UFIUBAPythonSubsystem::EndPlayInEditor()
{
	GEditor->RequestEndPlayMap();
	GUnrealEd->SetPIEWorldsPaused(false);
	FSlateApplication::Get().LeaveDebuggingMode(true);
}
#endif
