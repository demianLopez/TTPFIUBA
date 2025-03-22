// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class PythonLibrary : ModuleRules
{
	public PythonLibrary(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;
		PublicSystemIncludePaths.Add("$(ModuleDir)/include");

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			// Add the import library
			PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "x64", "Release", "python313.lib"));

			// Delay-load the DLL, so we can load it from the right place first
			//PublicDelayLoadDLLs.Add("ExampleLibrary.dll");

			// Ensure that the DLL is staged along with the executable
			//RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/PythonLibrary/Win64/ExampleLibrary.dll");
		}
	}
}
