// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CarrierStrikeCPP : ModuleRules
{
	public CarrierStrikeCPP(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			// Preprocessor�� ���⿡ �ϱ�� ���� �����ϼž� �˴ϴ�.
			PublicDefinitions.Add("UNICODE");
			PublicDefinitions.Add("_UNICODE");

			// ������� Include ��θ� ���⿡ �ϱ�� ���� �����մϴ�.
			PublicIncludePaths.Add(@"$(ProjectDir)\ProudNet\include");

			// ���̺귯�� ���� ��θ� �ϱ�� ���� �����մϴ�.
			PublicAdditionalLibraries.Add(@"$(ProjectDir)\ProudNet\lib\x64\v140\Release\ProudNetClient.lib");
			
		}
	}
}
