// Fill out your copyright notice in the Description page of Project Settings.

#include "FIUBAPythonTypes.h"
#include "Python.h"

FFPObjectContainer::~FFPObjectContainer()
{
	if (PyObjectRef != nullptr)
	{
		Py_DECREF(PyObjectRef);
	}
}

FFPyObjectPtr::FFPyObjectPtr(PyObject* Object)
{
	if (Object == nullptr)
	{
		PyErr_Print();
	}
	else
	{
		Container = MakeShareable(new FFPObjectContainer(Object));
	}
}

bool FFPyObjectPtr::IsValid() const
{
	return Container.IsValid() && Container->PyObjectRef != nullptr;
}

PyObject* FFPyObjectPtr::Get() const
{
	return Container->PyObjectRef;
}

void FFPyObjectPtr::Clear()
{
	Container.Reset();
}
