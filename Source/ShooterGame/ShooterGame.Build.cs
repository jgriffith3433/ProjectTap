// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ShooterGame : ModuleRules
{
	public ShooterGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateIncludePaths.AddRange(
			new string[] { 
				"ShooterGame/Private",
				"ShooterGame/Private/UI",
				"ShooterGame/Private/UI/Menu",
				"ShooterGame/Private/UI/Style",
				"ShooterGame/Private/UI/Widgets",
            }
		);

        PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine",
                "GameSparks",
                "OnlineSubsystem",
				"OnlineSubsystemUtils",
				"AssetRegistry",
				"NavigationSystem",
				"AIModule",
				"GameplayTasks",
			}
		);

        PrivateDependencyModuleNames.AddRange(
			new string[] {
				"InputCore",
                "UMG",
				"Slate",
				"SlateCore",
                "OnlineSubsystemGameSparks",
                "ShooterGameLoadingScreen",
				"Json",
				"ApplicationCore",
				"ReplicationGraph"
			}
		);

		DynamicallyLoadedModuleNames.AddRange(
			new string[] {
				//"OnlineSubsystemNull",
				"NetworkReplayStreaming",
				"NullNetworkReplayStreaming",
				"HttpNetworkReplayStreaming",
				"LocalFileNetworkReplayStreaming"
			}
		);

		PrivateIncludePathModuleNames.AddRange(
			new string[] {
				"NetworkReplayStreaming"
			}
		);

		if (Target.bBuildDeveloperTools || (Target.Configuration != UnrealTargetConfiguration.Shipping && Target.Configuration != UnrealTargetConfiguration.Test))
        {
            PrivateDependencyModuleNames.Add("GameplayDebugger");
            PublicDefinitions.Add("WITH_GAMEPLAY_DEBUGGER=1");
        }
		else
		{
			PublicDefinitions.Add("WITH_GAMEPLAY_DEBUGGER=0");
		}
	}
}
