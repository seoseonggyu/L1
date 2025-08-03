// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Logging/LogMacros.h"

class UObject;

L1GAME_API DECLARE_LOG_CATEGORY_EXTERN(LogL1, Log, All);
L1GAME_API DECLARE_LOG_CATEGORY_EXTERN(LogL1Experience, Log, All);
L1GAME_API DECLARE_LOG_CATEGORY_EXTERN(LogL1AbilitySystem, Log, All);
L1GAME_API DECLARE_LOG_CATEGORY_EXTERN(LogL1Teams, Log, All);

L1GAME_API FString GetClientServerContextString(UObject* ContextObject = nullptr);
