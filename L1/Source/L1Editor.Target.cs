// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class L1EditorTarget : TargetRules
{
	public L1EditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange(new string[] { "L1Game", "L1Editor" });

		if (!bBuildAllModules)
		{
			NativePointerMemberBehaviorOverride = PointerMemberBehavior.Disallow;
		}

		L1GameTarget.ApplySharedLyraTargetSettings(this);

		// This is used for touch screen development along with the "Unreal Remote 2" app
		EnablePlugins.Add("RemoteSession");
	}
}
