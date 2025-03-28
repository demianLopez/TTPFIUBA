// Fill out your copyright notice in the Description page of Project Settings.

#include "FIUBAPythonTypes.h"
#include "Python.h"

FFPObjectContainer::~FFPObjectContainer()
{
	Py_DECREF(PyObjectRef);
}

FFPyObjectPtr::FFPyObjectPtr(PyObject* Object)
{
	Container = MakeShareable(new FFPObjectContainer(Object));
}

FFPyObjectPtr::operator PyObject*() const
{
	return Container->PyObjectRef;
}

void FFPyObjectPtr::Clear()
{
	Container.Reset();
}
