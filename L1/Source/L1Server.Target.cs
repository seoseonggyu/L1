// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms(UnrealPlatformClass.Server)]
public class L1ServerTarget : TargetRules
{
	public L1ServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;

		ExtraModuleNames.AddRange(new string[] { "L1Game" });

		L1GameTarget.ApplySharedLyraTargetSettings(this);

		bUseChecksInShipping = true;
	}
}
