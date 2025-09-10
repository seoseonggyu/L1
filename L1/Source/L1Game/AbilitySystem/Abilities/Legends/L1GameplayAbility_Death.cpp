#include "L1GameplayAbility_Death.h"

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Character/LyraHealthComponent.h"
#include "L1GameplayTags.h"
#include "L1LogChannels.h"
#include "Trace/Trace.inl"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1GameplayAbility_Death)

UL1GameplayAbility_Death::UL1GameplayAbility_Death(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityTags.AddTag(L1GameplayTags::Ability_Death);
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = L1GameplayTags::GameplayEvent_Death;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}

void UL1GameplayAbility_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	check(ActorInfo);
	
	ULyraAbilitySystemComponent* LyraASC = CastChecked<ULyraAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());

	FGameplayTagContainer AbilityTypesToIgnore;
	AbilityTypesToIgnore.AddTag(L1GameplayTags::Ability_Behavior_SurvivesDeath);
	
	LyraASC->CancelAbilities(nullptr, &AbilityTypesToIgnore, this);
	SetCanBeCanceled(false);

	if (ChangeActivationGroup(ELyraAbilityActivationGroup::Exclusive_Blocking) == false)
	{
		UE_LOG(LogL1AbilitySystem, Error, TEXT("UL1GameplayAbility_Death::ActivateAbility: Ability [%s] failed to change activation group to blocking."), *GetName());
	}

	SetCameraMode(DeathCameraModeClass);
	
	if (bAutoStartDeath)
	{
		StartDeath();
	}

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}, EndDelay, false);
}

void UL1GameplayAbility_Death::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	check(ActorInfo);
	
	FinishDeath();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UL1GameplayAbility_Death::StartDeath()
{
	if (ULyraHealthComponent* HealthComponent = ULyraHealthComponent::FindHealthComponent(GetAvatarActorFromActorInfo()))
	{
		if (HealthComponent->GetDeathState() == ELyraDeathState::NotDead)
		{
			HealthComponent->StartDeath();
		}
	}
}

void UL1GameplayAbility_Death::FinishDeath()
{
	if (ULyraHealthComponent* HealthComponent = ULyraHealthComponent::FindHealthComponent(GetAvatarActorFromActorInfo()))
	{
		if (HealthComponent->GetDeathState() == ELyraDeathState::DeathStarted)
		{
			HealthComponent->FinishDeath();
		}
	}
}
