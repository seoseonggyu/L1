#include "L1GameplayAbility_Interact_Object.h"

#include "L1GameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Actors/L1EquipmentBase.h"
#include "Character/LyraCharacter.h"
#include "Interaction/L1Interactable.h"
#include "Interaction/L1WorldInteractable.h"
#include "Item/Managers/L1EquipManagerComponent.h"
#include "Tasks/L1AbilityTask_WaitForInvalidInteraction.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1GameplayAbility_Interact_Object)

UL1GameplayAbility_Interact_Object::UL1GameplayAbility_Interact_Object(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    ActivationPolicy = ELyraAbilityActivationPolicy::Manual;

	AbilityTags.AddTag(L1GameplayTags::Ability_Interact_Object);
	ActivationOwnedTags.AddTag(L1GameplayTags::Status_Interact);
}

void UL1GameplayAbility_Interact_Object::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TriggerEventData == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	AActor* TargetActor = const_cast<AActor*>(TriggerEventData->Target.Get());
	if (InitializeAbility(TargetActor) == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	bInitialized = false;

	FL1InteractionQuery Query;
	Query.RequestingAvatar = GetAvatarActorFromActorInfo();
	Query.RequestingController = GetControllerFromActorInfo();

	if (Interactable->CanInteraction(Query) == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo) == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	bInitialized = true;

	if (AL1WorldInteractable* WorldInteractable = Cast<AL1WorldInteractable>(TargetActor))
	{
		WorldInteractable->OnInteractionSuccess(GetAvatarActorFromActorInfo());
	}
	
	if (UL1AbilityTask_WaitForInvalidInteraction* InvalidInteractionTask = UL1AbilityTask_WaitForInvalidInteraction::WaitForInvalidInteraction(this, AcceptanceAngle, AcceptanceDistance))
	{
		InvalidInteractionTask->OnInvalidInteraction.AddDynamic(this, &ThisClass::OnInvalidInteraction);
		InvalidInteractionTask->ReadyForActivation();
	}

	if (UAnimMontage* ActiveEndMontage = InteractionInfo.ActiveEndMontage)
	{
		if (UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("InteractMontage"), ActiveEndMontage, 1.f, NAME_None, true, 1.f, 0.f, false))
		{
			PlayMontageTask->ReadyForActivation();
		}
	}
}

void UL1GameplayAbility_Interact_Object::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (InteractionInfo.Duration > 0.f)
	{
		if (ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo())
		{
			if (UL1EquipManagerComponent* EquipManager = LyraCharacter->GetComponentByClass<UL1EquipManagerComponent>())
			{
				EquipManager->ChangeShouldHiddenEquipments(false);

				if (EquipManager->GetCurrentEquipState() != EEquipState::Unarmed)
				{
					if (AL1EquipmentBase* EquipmentActor = EquipManager->GetFirstEquippedActor())
					{
						EquipmentActor->PlayEquipMontage();
					}
				}
			}
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UL1GameplayAbility_Interact_Object::OnInvalidInteraction()
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}
