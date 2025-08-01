// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class L1ClientTarget : TargetRules
{
	public L1ClientTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Client;

		ExtraModuleNames.AddRange(new string[] { "L1Game" });

		L1GameTarget.ApplySharedLyraTargetSettings(this);
	}
}
