#include "L1GameplayAbility_Interact.h"

#include "AbilitySystemComponent.h"
#include "L1GameplayTags.h"
#include "AbilitySystem/Abilities/Tasks/L1AbilityTask_WaitInputStart.h"
#include "Character/LyraCharacter.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Interaction/L1InteractionInfo.h"
#include "Physics/LyraCollisionChannels.h"
#include "Tasks/L1AbilityTask_GrantNearbyInteraction.h"
#include "Tasks/L1AbilityTask_WaitForInteractableTraceHit.h"
#include "UI/IndicatorSystem/LyraIndicatorManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1GameplayAbility_Interact)

UL1GameplayAbility_Interact::UL1GameplayAbility_Interact(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = ELyraAbilityActivationPolicy::OnSpawn;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UL1GameplayAbility_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FL1InteractionQuery InteractionQuery;
	InteractionQuery.RequestingAvatar = GetAvatarActorFromActorInfo();
	InteractionQuery.RequestingController = GetControllerFromActorInfo();
	
	if (UL1AbilityTask_WaitForInteractableTraceHit* TraceHitTask = UL1AbilityTask_WaitForInteractableTraceHit::WaitForInteractableTraceHit(this, InteractionQuery, L1_TraceChannel_Interaction, InteractionTraceRate, bShowTraceDebug))
	{
		TraceHitTask->ReadyForActivation();
	}
}