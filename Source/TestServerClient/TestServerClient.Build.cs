// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TestServerClient : ModuleRules
{
	public TestServerClient(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay","UMG" }); 
	
		PublicIncludePaths.AddRange(new string[] { 
		"TestServerClient/Public/Blaster/Player",
        "TestServerClient/Public/Blaster/GameModes",
        "TestServerClient/Public/Blaster/HUD",
        "TestServerClient/Public/Blaster/Weapons",
        "TestServerClient/Public/Blaster/BlasterComponents",
        "TestServerClient/Public/Blaster/PlayerController",
        "TestServerClient/Public/Blaster/Interfaces"
        });


    }
}
