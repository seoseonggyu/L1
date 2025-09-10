﻿#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "L1AbilityTask_WaitForTick.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTickTaskDelegate, float, DeltaTime);

UCLASS()
class UL1AbilityTask_WaitForTick : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UL1AbilityTask_WaitForTick(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="true"))
	static UL1AbilityTask_WaitForTick* WaitForTick(UGameplayAbility* OwningAbility);

protected:
	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

public:
	UPROPERTY(BlueprintAssignable)
	FOnTickTaskDelegate OnTick;
};
