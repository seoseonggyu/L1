// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class L1ServerEOSTarget : L1ServerTarget
{
	public L1ServerEOSTarget(TargetInfo Target) : base(Target)
	{
		CustomConfig = "EOS";
	}
}
