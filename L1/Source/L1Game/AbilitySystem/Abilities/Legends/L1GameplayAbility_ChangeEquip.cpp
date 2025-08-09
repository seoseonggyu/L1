#include "L1GameplayAbility_ChangeEquip.h"

#include "L1GameplayTags.h"
#include "Character/LyraCharacter.h"
#include "Item/Managers/L1EquipManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1GameplayAbility_ChangeEquip)

UL1GameplayAbility_ChangeEquip::UL1GameplayAbility_ChangeEquip(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    ActivationPolicy = ELyraAbilityActivationPolicy::Manual;
	ActivationGroup = ELyraAbilityActivationGroup::Exclusive_Blocking;
	bServerRespectsRemoteAbilityCancellation = false;

	AbilityTags.AddTag(L1GameplayTags::Ability_ChangeEquip);
	ActivationOwnedTags.AddTag(L1GameplayTags::Status_ChangeEquip);

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = L1GameplayTags::GameplayEvent_ChangeEquip;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}

void UL1GameplayAbility_ChangeEquip::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TriggerEventData == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo();
	if (LyraCharacter == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	UL1EquipManagerComponent* EquipManager = LyraCharacter->GetComponentByClass<UL1EquipManagerComponent>();
	if (EquipManager == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	int32 NewEquipStateIndex = FMath::RoundToInt(TriggerEventData->EventMagnitude);
	EEquipState NewEquipState = (EEquipState)NewEquipStateIndex;

	if (EquipManager->CanChangeEquipState(NewEquipState))
	{
		EquipManager->ChangeEquipState(NewEquipState);
		
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		}, EndDelay, false);
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}
