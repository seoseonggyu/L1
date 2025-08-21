// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "L1NetworkPawnData.generated.h"

class ULyraAbilitySet;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "L1|Abilities")
	TArray<TObjectPtr<ULyraAbilitySet>> AbilitySets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "L1|Widget") // TODO: 임시로 여기다가 사용, 따로 옮겨야 함
	TSubclassOf<UUserWidget> WidgetClass; 
};
