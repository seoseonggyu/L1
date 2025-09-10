﻿#include "L1AbilityTask_WaitInputStart.h"

#include "AbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1AbilityTask_WaitInputStart)

UL1AbilityTask_WaitInputStart::UL1AbilityTask_WaitInputStart(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

UL1AbilityTask_WaitInputStart* UL1AbilityTask_WaitInputStart::WaitInputStart(UGameplayAbility* OwningAbility)
{
	UL1AbilityTask_WaitInputStart* Task = NewAbilityTask<UL1AbilityTask_WaitInputStart>(OwningAbility);
	return Task;
}

void UL1AbilityTask_WaitInputStart::Activate()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC && Ability)
	{
		DelegateHandle = ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey()).AddUObject(this, &UL1AbilityTask_WaitInputStart::OnStartCallback);
		if (IsForRemoteClient())
		{
			if (ASC->CallReplicatedEventDelegateIfSet(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey()) == false)
			{
				SetWaitingOnRemotePlayerData();
			}
		}
	}
}

void UL1AbilityTask_WaitInputStart::OnStartCallback()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (Ability == nullptr || ASC == nullptr)
		return;

	ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey()).Remove(DelegateHandle);

	FScopedPredictionWindow ScopedPrediction(ASC, IsPredictingClient());

	if (IsPredictingClient())
	{
		ASC->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey(), ASC->ScopedPredictionKey);
	}
	else
	{
		ASC->ConsumeGenericReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey());
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnStart.Broadcast();	
	}
	EndTask();
}
