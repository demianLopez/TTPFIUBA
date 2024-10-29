// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FiubaDefenseEditor : ModuleRules
{
	public FiubaDefenseEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicIncludePaths.AddRange(new string[] { "FiubaDefense", "FiubaDefenseEditor" });
		PublicDependencyModuleNames.AddRange(new string[] {  });
		PrivateDependencyModuleNames.AddRange(new string[] { "AssetTools", "ToolMenus", "UnrealEd", "GraphEditor",
			"RenderCore", "FiubaDefense", "Core", "CoreUObject", "Engine", "InputCore", "UMG", "Slate", "SlateCore", "PropertyEditor", "EditorStyle" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");


		
		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
