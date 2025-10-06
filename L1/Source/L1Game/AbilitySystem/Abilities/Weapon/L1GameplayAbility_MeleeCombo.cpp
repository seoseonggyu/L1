#include "L1GameplayAbility_MeleeCombo.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitConfirmCancel.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/Tasks/L1AbilityTask_WaitInputStart.h"
#include "Item/Managers/L1EquipManagerComponent.h"
#include "System/LyraGameData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1GameplayAbility_MeleeCombo)

UL1GameplayAbility_MeleeCombo::UL1GameplayAbility_MeleeCombo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UL1GameplayAbility_MeleeCombo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	bInputPressed = false;
	bInputReleased = false;

	if (UAbilityTask_WaitInputRelease* InputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, true))
	{
		InputReleaseTask->OnRelease.AddDynamic(this, &ThisClass::OnInputReleased);
		InputReleaseTask->ReadyForActivation();
	}

	WaitInputContinue();
	WaitInputStop();
}

void UL1GameplayAbility_MeleeCombo::HandleMontageEvent(FGameplayEventData Payload)
{

	bool bCanContinue = NextAbilityClass && (bInputPressed || bInputReleased == false) && (bBlocked == false);
	if (bCanContinue)
	{
		if (UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo())
		{
			AbilitySystemComponent->TryActivateAbilityByClass(NextAbilityClass);
		}
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
	else if (bBlocked == false)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UL1GameplayAbility_MeleeCombo::WaitInputContinue()
{
	if (UL1AbilityTask_WaitInputStart* InputStartTask = UL1AbilityTask_WaitInputStart::WaitInputStart(this))
	{
		InputStartTask->OnStart.AddDynamic(this, &ThisClass::OnInputStart);
		InputStartTask->ReadyForActivation();
	}
}

void UL1GameplayAbility_MeleeCombo::WaitInputStop()
{
	if (UAbilityTask_WaitConfirmCancel* InputConfirmCancelTask = UAbilityTask_WaitConfirmCancel::WaitConfirmCancel(this))
	{
		InputConfirmCancelTask->OnCancel.AddDynamic(this, &ThisClass::OnInputCancel);
		InputConfirmCancelTask->ReadyForActivation();
	}
}

void UL1GameplayAbility_MeleeCombo::OnInputReleased(float TimeHeld)
{
	bInputReleased = true;
}

void UL1GameplayAbility_MeleeCombo::OnInputStart()
{
	bInputPressed = true;
	WaitInputContinue();
}

void UL1GameplayAbility_MeleeCombo::OnInputCancel()
{
	bInputPressed = false;
	WaitInputStop();
}
