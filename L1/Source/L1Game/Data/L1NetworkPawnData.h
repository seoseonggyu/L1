// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "L1NetworkPawnData.generated.h"

/**
 * 
 */
UCLASS()
class L1GAME_API UL1NetworkPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

	UL1NetworkPawnData(const FObjectInitializer& ObjectInitializer);

public:
	static const UL1NetworkPawnData& Get();

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "L1|NetworkPawn")
	TSubclassOf<APawn> PawnClass;
};
