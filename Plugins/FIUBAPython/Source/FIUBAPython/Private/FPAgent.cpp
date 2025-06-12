// Fill out your copyright notice in the Description page of Project Settings.


#include "FPAgent.h"
#include "Python.h"
#include "FIUBAPythonTypes.h"

DEFINE_LOG_CATEGORY(FDPythonLog);

PyObject* CallbackDesdePython(PyObject* self, PyObject* args)
{
	PyObject* pyObj;
	const char* mensaje;
	
	if (!PyArg_ParseTuple(args, "Os", &pyObj, &mensaje)) {
		PyErr_SetString(PyExc_RuntimeError, "Argumentos inválidos");
		return nullptr;
	}

	FString Message(UTF8_TO_TCHAR(mensaje));
	UE_LOG(FDPythonLog, Warning, TEXT("%s"), *Message);
	
	Py_RETURN_NONE;
}

void UFPAgent::InitAgent(const FString& InAgentName, int32 InStateDim, int32 ActionStateDim)
{
	ensure(!bTrainInitialized);

	StateDim = InStateDim;
	AgentName = InAgentName;

	FFPyObjectPtr pModule = PyImport_ImportModule("rl_framework.rl_framework");	
	FFPyObjectPtr PyObjectDictionary = PyModule_GetDict(pModule.Get());	
	FFPyObjectPtr python_class = PyDict_GetItemString(PyObjectDictionary.Get(), "RLFramework");

	if (PyCallable_Check(python_class.Get()))
	{
		Container = MakeShared<FFPObjectContainer>(PyObject_CallObject(python_class.Get(), nullptr));
	}
	
	callbackDef = new PyMethodDef();
	callbackDef->ml_name = "cpp_callback";
	callbackDef->ml_meth = CallbackDesdePython;
	callbackDef->ml_flags = METH_VARARGS;
	callbackDef->ml_doc = "Callback hacia C++ desde Python";

	PyObject* method = PyCFunction_NewEx(callbackDef, nullptr, nullptr);
	PyObject* bound_method = PyMethod_New(method, Container->PyObjectRef);
	
	PyObject* dict = PyObject_GetAttrString(Container->PyObjectRef, "__dict__");
	PyDict_SetItemString(dict, "cpp_callback", bound_method);
	
	FFPyObjectPtr PyInitialState = PyList_New(StateDim);
	for (int32 i = 0; i < StateDim; i++)
	{
		PyList_SetItem(PyInitialState.Get(), i, PyFloat_FromDouble(0.0f));
	}

	FFPyObjectPtr Config = PyDict_New();

	FFPyObjectPtr Policy = PyDict_New();
	PyDict_SetItemString(Policy.Get(), "epsilon", PyFloat_FromDouble(1.0));
	PyDict_SetItemString(Policy.Get(), "epsilon_min", PyFloat_FromDouble(0.01));
	PyDict_SetItemString(Policy.Get(), "epsilon_decay", PyFloat_FromDouble(0.995));

	FFPyObjectPtr Agent = PyDict_New();
	PyDict_SetItemString(Agent.Get(), "gamma", PyFloat_FromDouble(0.99));
	PyDict_SetItemString(Agent.Get(), "learning_rate", PyFloat_FromDouble(0.001));
	PyDict_SetItemString(Agent.Get(), "batch_size", PyLong_FromLong(32));
	PyDict_SetItemString(Agent.Get(), "replay_buffer", PyLong_FromLong(10000));

	PyDict_SetItemString(Config.Get(), "policy", Policy.Get());
	PyDict_SetItemString(Config.Get(), "agent", Agent.Get());
	
	FFPyObjectPtr py_string = PyUnicode_FromString(TCHAR_TO_UTF8(*InAgentName));	
	FFPyObjectPtr args = Py_BuildValue("(OOiO)", py_string.Get(), PyInitialState.Get(), ActionStateDim, Config.Get());
	
	FFPyObjectPtr InitTrainValue = PyObject_CallMethod(Container->PyObjectRef, "initialize_train", "O", args.Get());
	
	bTrainInitialized = true;	
	ensure(InitTrainValue.IsValid());
}

int32 UFPAgent::Train(const TArray<float>& State, float Reward, bool bInEpisodeFinished)
{
	if (!ensure(!bEpisodeFinished))
	{
		return -1;
	}
	
	FFPyObjectPtr PyState = PyList_New(State.Num());

	ensure(StateDim == State.Num());
	
	int Index = 0;
	for (float StateValue : State)
	{
		PyList_SetItem(PyState.Get(), Index, PyFloat_FromDouble(StateValue));
		Index++;
	}
		
	FFPyObjectPtr PyEpisodeFinished = PyBool_FromLong(bInEpisodeFinished ? 1 : 0);

	FFPyObjectPtr PyReward = PyFloat_FromDouble(Reward);
	
	FFPyObjectPtr args = Py_BuildValue("(OOO)", PyState.Get(), PyReward.Get(), PyEpisodeFinished.Get());	
	FFPyObjectPtr TrainValue = PyObject_CallMethod(Container->PyObjectRef, "train", "O", args.Get());

	MeanReward += Reward;

	bEpisodeFinished = bInEpisodeFinished;
	if (TrainValue.IsValid())
	{
		if (bEpisodeFinished)
		{
			EpisodeFinished();
		}

		FFPyObjectPtr TakenAction = PyTuple_GetItem(TrainValue.Get(), 0);
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

void UFPAgent::EpisodeFinished()
{
	OnEpisodeFinished.Broadcast();
}

void UFPAgent::EpisodeStart()
{
	bEpisodeFinished = false;
	MeanReward = 0.0f;
}
