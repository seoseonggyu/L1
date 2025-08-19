// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "L1GameplayEffect_InitStat.generated.h"

/**
 * 
 */
UCLASS()
class L1GAME_API UL1GameplayEffect_InitStat : public UGameplayEffect
{
	GENERATED_BODY()
	
public:
	UL1GameplayEffect_InitStat(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

};