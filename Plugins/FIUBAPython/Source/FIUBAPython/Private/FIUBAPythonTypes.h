// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "pytypedefs.h"

struct FFPObjectContainer
{
	PyObject* PyObjectRef;

	FFPObjectContainer(PyObject* Object) : PyObjectRef(Object) {};
	~FFPObjectContainer();
};

struct FFPyObjectPtr
{
public:

	FFPyObjectPtr() {}
	FFPyObjectPtr(PyObject* Object);

	PyObject* Get() const;
	void Clear();

	bool IsValid() const;
protected:

	TSharedPtr<FFPObjectContainer> Container;
};
