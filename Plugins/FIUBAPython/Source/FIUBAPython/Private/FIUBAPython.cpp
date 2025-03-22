// Copyright Epic Games, Inc. All Rights Reserved.

#include "FIUBAPython.h"

#include "Modules/ModuleManager.h"
#include "Python.h"

#define LOCTEXT_NAMESPACE "FFIUBAPythonModule"

void FFIUBAPythonModule::StartupModule()
{
	RunTest();
}

void FFIUBAPythonModule::ShutdownModule()
{

}

PyObject* CallbackDesdePython(PyObject* self, PyObject* args)
{
	PyObject* pyObj;
	const char* mensaje;

	// Extrae los argumentos: (objeto Python, mensaje)
	if (!PyArg_ParseTuple(args, "Os", &pyObj, &mensaje)) {
		PyErr_SetString(PyExc_RuntimeError, "Argumentos inválidos");
		return nullptr;
	}

	// Obtén el puntero C++ desde el atributo "__cpp_object__"
	PyObject* cppObjectAttr = PyDict_GetItemString(pyObj, "__cpp_object__");
	if (!cppObjectAttr || !PyCapsule_CheckExact(cppObjectAttr)) {
		PyErr_SetString(PyExc_RuntimeError, "No se encontró el puntero al objeto C++");
		return nullptr;
	}

	// Convierte el PyCapsule a un puntero C++
	FFIUBAPythonModule* cppObject = static_cast<FFIUBAPythonModule*>(PyCapsule_GetPointer(cppObjectAttr, "FFIUBAPythonModule"));
	if (!cppObject) {
		PyErr_SetString(PyExc_RuntimeError, "El puntero al objeto C++ es inválido");
		return nullptr;
	}
	
	Py_RETURN_NONE;
}

void FFIUBAPythonModule::RunTest()
{
	Py_Initialize();
	
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.insert(0, 'D:/FIUBASERVER/fiubaenv/Lib/site-packages')");

	PyRun_SimpleString("import os");
	PyRun_SimpleString("os.chdir('D:/FIUBASERVER/main')");
	
	FString ImportString = "sys.path.append('D:/FIUBASERVER/main')";
	PyRun_SimpleString(TCHAR_TO_UTF8(*ImportString));
	
	PyObject* pModule = PyImport_ImportModule("main");
	PyObject* pFunc = PyObject_GetAttrString(pModule, "production");
	PyObject* pArgs = PyTuple_Pack(1, PyUnicode_FromString("state.json"));

	PyObject* pResult = PyObject_CallObject(pFunc, pArgs);
	if (!pResult) {
		PyErr_Print();
	} else {
		// Convertir el resultado de Python a C++ (asumiendo que es un string)
		const char* result = PyUnicode_AsUTF8(pResult);
		// Liberar memoria
		Py_DECREF(pResult);
	}
	
	Py_DECREF(pFunc);
	

	Py_DECREF(pModule);
	

	Py_Finalize();
}


void FFIUBAPythonModule::RunTest2()
{	
	Py_Initialize();
	
	PyRun_SimpleString("import sys");


	FString ImportString = "sys.path.append('" + FPaths::ProjectContentDir() + "Scripts')";
	PyRun_SimpleString(TCHAR_TO_UTF8(*ImportString));
	
	// Prepara los argumentos para la función (en este caso, una cadena)
	PyObject* pArgs = PyTuple_Pack(1, PyUnicode_FromString("Mundo"));

	// Llama a la función y obtiene el resultado

	auto CppCallback = [](PyObject* args) -> PyObject*
	{
		const char* msg = nullptr;

		if (PyArg_ParseTuple(args, "s", &msg))
		{
			UE_LOG(LogTemp, Warning, TEXT("Mensaje desde python con callback %hs"), msg);
		};
		Py_RETURN_NONE;				
	};

	PyObject* cppObjectCapsule = PyCapsule_New(static_cast<void*>(this), "FFIUBAPythonModule", nullptr);
	PyObject* pyObj = PyDict_New();
	int asd = PyDict_SetItemString(pyObj, "__cpp_object__", cppObjectCapsule);

	PyMethodDef callbackDef = {
		"cpp_callback",                     // Nombre del método
		CallbackDesdePython,          // Puntero a la función
		METH_VARARGS,                      // Indica que acepta argumentos
		"Callback hacia C++ desde Python"  // Descripción
	};

	PyObject* pyCallback = PyCFunction_New(&callbackDef, nullptr);
	
	PyObject* pModule = PyImport_ImportModule("script");
	{
		PyObject* mainDict = PyModule_GetDict(pModule);
		PyDict_SetItemString(mainDict, "cpp_obj", pyObj);
		PyDict_SetItemString(mainDict, "cpp_callback", pyCallback);
	}
	
	PyObject* pFunc = PyObject_GetAttrString(pModule, "saludar");

	PyObject* pValue = PyObject_CallObject(pFunc, pArgs);
	Py_DECREF(pArgs);

	if (pValue != nullptr) {
		// Convierte el resultado de Python (un string) a C++
		const char* result = PyUnicode_AsUTF8(pValue);
		UE_LOG(LogTemp, Warning, TEXT("%hs"), result);

		Py_DECREF(pValue);
	}

	Py_DECREF(pFunc);
	

	Py_DECREF(pModule);
	

	Py_Finalize();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FFIUBAPythonModule, FIUBAPython)
