// Fill out your copyright notice in the Description page of Project Settings.

#include "FIUBAPythonSubsystem.h"
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

void UFIUBAPythonSubsystem::ExecuteTraining(int32 NumberOfMatches)
{
	StartAutonomousTraining(NumberOfMatches);

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

int32 UFIUBAPythonSubsystem::InitializeTrain(const TArray<float>& Values, int32 ActionStateDim)
{
	if (bTrainInitialized)
		return 0;

	bTrainInitialized = true;
	
	InitPython();

	FFPyObjectPtr pModule = PyImport_ImportModule("rl_framework.rl_framework");	
	FFPyObjectPtr PyObjectDictionary = PyModule_GetDict(pModule.Get());	
	FFPyObjectPtr python_class = PyDict_GetItemString(PyObjectDictionary.Get(), "RLFramework");

	if (PyCallable_Check(python_class.Get()))
	{
		FrameworkPythonObject = PyObject_CallObject(python_class.Get(), nullptr);
	}

	// Create Array
	FFPyObjectPtr PyInitialState = PyList_New(Values.Num());
	int Index = 0;
	for (float Value : Values)
	{
		PyList_SetItem(PyInitialState.Get(), Index, PyFloat_FromDouble(Value));
		Index++;
	}

	FFPyObjectPtr Config = PyDict_New();

	FFPyObjectPtr Policy = PyDict_New();
	PyDict_SetItemString(Policy.Get(), "epsilon", PyLong_FromLong(1.0));
	PyDict_SetItemString(Policy.Get(), "epsilon_min", PyLong_FromLong(0.01));
	PyDict_SetItemString(Policy.Get(), "epsilon_decay", PyLong_FromLong(0.995));

	FFPyObjectPtr Agent = PyDict_New();
	PyDict_SetItemString(Agent.Get(), "gamma", PyLong_FromLong(0.99));
	PyDict_SetItemString(Agent.Get(), "learning_rate", PyLong_FromLong(0.001));
	PyDict_SetItemString(Agent.Get(), "batch_size", PyLong_FromLong(32));
	PyDict_SetItemString(Agent.Get(), "replay_buffer", PyLong_FromLong(10000));

	PyDict_SetItemString(Config.Get(), "policy", Policy.Get());
	PyDict_SetItemString(Config.Get(), "agent", Agent.Get());

	
	// Build argument tuple: (agent_id, initial_state, action_state_dim)
	FFPyObjectPtr py_string = PyUnicode_FromString("UnrealAgent");	
	FFPyObjectPtr args = Py_BuildValue("(OOiO)", py_string.Get(), PyInitialState.Get(), ActionStateDim, Config.Get());
	
	FFPyObjectPtr InitTrainValue = PyObject_CallMethod(FrameworkPythonObject.Get(), "initialize_train", "O", args.Get());
		
	if (InitTrainValue.IsValid())
	{
		return 0;
	}

	ensure(false);
	return -1;
}

int32 UFIUBAPythonSubsystem::Train(const TArray<float>& Values, const TArray<FFIRewardValues>& MaxValues, const TArray<FFIRewardValues>& MinValues, bool FinishTrain, bool FinishLoop)
{
	FFPyObjectPtr State = PyList_New(Values.Num());

	int Index = 0;
	for (float Value : Values)
	{
		PyList_SetItem(State.Get(), Index, PyFloat_FromDouble(Value));
		Index++;
	}

	// REWARD !!!!
	FFPyObjectPtr RewardData = PyDict_New();
	
	FFPyObjectPtr MaxVars = PyList_New(MaxValues.Num());
	if (MaxValues.Num() > 0)
	{
		Index = 0;
		for (FFIRewardValues Value : MaxValues)
		{
			PyList_SetItem(MaxVars.Get(), Index, PyLong_FromLong(Value.Value));
			Index++;
		}
	}
	
	FFPyObjectPtr MinVars = PyList_New(MinValues.Num());
	if (MinValues.Num() > 0)
	{
		Index = 0;
		for (FFIRewardValues Value : MinValues)
		{
			PyList_SetItem(MinVars.Get(), Index, PyLong_FromLong(Value.Value));
			Index++;
		}
	}
	
	PyDict_SetItemString(RewardData.Get(), "max_vars", MaxVars.Get());
	PyDict_SetItemString(RewardData.Get(), "min_vars", MinVars.Get());
	
	// -- REWARD!!!!
	
	FFPyObjectPtr PyFinishLoop = PyBool_FromLong(FinishLoop ? 1 : 0);  // false
	FFPyObjectPtr PyFinsihTrain = PyBool_FromLong(FinishTrain ? 1 : 0); // false
	
	FFPyObjectPtr args = Py_BuildValue("(OOOO)", State.Get(), RewardData.Get(), PyFinishLoop.Get(), PyFinsihTrain.Get());	
	FFPyObjectPtr TrainValue = PyObject_CallMethod(FrameworkPythonObject.Get(), "train", "O", args.Get());

	if (TrainValue.IsValid())
	{
		if (FinishLoop)
		{
			EndCurrentMatch();
		}

		FFPyObjectPtr TakenAction = PyTuple_GetItem(TrainValue.Get(), 0);  // No aumenta la ref
		FFPyObjectPtr LastAction = PyTuple_GetItem(TrainValue.Get(), 1);  

		if (TakenAction.Get() != Py_None)
		{
			return PyLong_AsLong(TakenAction.Get());
		}

		return 0;
	}

	ensure(false);
	return -1;
}

/* initialize_train(self, agent_id, initial_state, action_state_dim: int):
 * */

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

void UFIUBAPythonSubsystem::EndCurrentMatch()
{
	if (!IsPerformingAutonomousTraining())
		return;
	
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
