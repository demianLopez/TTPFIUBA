// Fill out your copyright notice in the Description page of Project Settings.

#include "FIUBAPythonSubsystem.h"
#include "Python.h"

UFIUBAPythonSubsystem& UFIUBAPythonSubsystem::Get()
{
	return *GEngine->GetEngineSubsystem<UFIUBAPythonSubsystem>();
}

void UFIUBAPythonSubsystem::InitModel()
{
	InitPython();

	FFPyObjectPtr pModule = PyImport_ImportModule("main");	
	FFPyObjectPtr PyObjectDictionary = PyModule_GetDict(pModule);	
	FFPyObjectPtr python_class = PyDict_GetItemString(PyObjectDictionary, "Framework");

	if (PyCallable_Check(python_class))
	{
		FrameworkPythonObject = PyObject_CallObject(python_class, nullptr);
	}
	
	FFPyObjectPtr InitTrainValue = PyObject_CallMethod(FrameworkPythonObject, "initialize_train", "(i)", 9);	
}

void UFIUBAPythonSubsystem::Train()
{
	FFPyObjectPtr TrainValue = PyObject_CallMethod(FrameworkPythonObject, "train", NULL);
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
