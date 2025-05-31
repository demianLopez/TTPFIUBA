// Fill out your copyright notice in the Description page of Project Settings.

#include "FIUBAPythonInterface.h"

#include "FIUBAPythonSubsystem.h"

IFIUBAPythonInterface& IFIUBAPythonInterface::Get()
{
	return UFIUBAPythonSubsystem::Get();
}