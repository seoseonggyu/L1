#include "L1GameplayAbility_Equipment.h"

#include "AbilitySystemGlobals.h"
#include "Abilities/Async/AbilityAsync_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Actors/L1EquipmentBase.h"
#include "Character/LyraCharacter.h"
#include "Item/L1ItemInstance.h"
#include "Item/Fragments/L1ItemFragment_Equipable_Utility.h"
#include "Item/Fragments/L1ItemFragment_Equipable_Weapon.h"
#include "Item/Managers/L1EquipManagerComponent.h"
#include "System/LyraGameData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1GameplayAbility_Equipment)

UL1GameplayAbility_Equipment::UL1GameplayAbility_Equipment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UL1GameplayAbility_Equipment::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(ActorInfo->AvatarActor.Get());
	if (LyraCharacter == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	UL1EquipManagerComponent* EquipManager = LyraCharacter->FindComponentByClass<UL1EquipManagerComponent>();
	if (EquipManager == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	UL1ItemInstance* ItemInstance = nullptr;
	
	for (FL1EquipmentInfo& EquipmentInfo : EquipmentInfos)
	{
		switch (EquipmentInfo.EquipmentType)
		{
		case EEquipmentType::Armor:
			ItemInstance = EquipManager->GetEquippedItemInstance(EquipmentInfo.RequiredArmorType);
			break;
		case EEquipmentType::Weapon:
			ItemInstance = EquipManager->GetEquippedItemInstance(EquipmentInfo.WeaponHandType);
			EquipmentInfo.EquipmentActor = EquipManager->GetEquippedActor(EquipmentInfo.WeaponHandType);
			break;
		case EEquipmentType::Utility:
			ItemInstance = EquipManager->GetFirstEquippedItemInstance();
			EquipmentInfo.EquipmentActor = EquipManager->GetFirstEquippedActor();
			break;
		}
		
		if ((ItemInstance == nullptr) || (EquipmentInfo.EquipmentType != EEquipmentType::Armor && EquipmentInfo.EquipmentActor == nullptr))
		{
			CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
			return;
		}
	}
	
	SnapshottedAttackRate = DefaultAttackRate;
	// SSG: 
	/*if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (const UL1CombatSet* CombatSet = Cast<UL1CombatSet>(ASC->GetAttributeSet(UL1CombatSet::StaticClass())))
		{
			float AttackSpeedPercent = CombatSet->GetAttackSpeedPercent();
			SnapshottedAttackRate = DefaultAttackRate + (DefaultAttackRate * (AttackSpeedPercent / 100.f));
		}
	}*/
}

bool UL1GameplayAbility_Equipment::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags) == false)
		return false;

	ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(ActorInfo->AvatarActor.Get());
	if (LyraCharacter == nullptr)
		return false;

 	UL1EquipManagerComponent* EquipManager = LyraCharacter->FindComponentByClass<UL1EquipManagerComponent>();
	if (EquipManager == nullptr)
		return false;
	
	for (const FL1EquipmentInfo& EquipmentInfo : EquipmentInfos)
	{
		if (EquipmentInfo.EquipmentType == EEquipmentType::Count)
			return false;

		if (EquipmentInfo.EquipmentType == EEquipmentType::Armor)
		{
			UL1ItemInstance* ItemInstance = EquipManager->GetEquippedItemInstance(EquipmentInfo.RequiredArmorType);
			if (ItemInstance == nullptr)
				return false;
		}
		else if (EquipmentInfo.EquipmentType == EEquipmentType::Weapon)
		{
			UL1ItemInstance* ItemInstance = EquipManager->GetEquippedItemInstance(EquipmentInfo.WeaponHandType);
			if (ItemInstance == nullptr)
				return false;

			const UL1ItemFragment_Equipable_Weapon* WeaponFragment = ItemInstance->FindFragmentByClass<UL1ItemFragment_Equipable_Weapon>();
			if (WeaponFragment == nullptr)
				return false;
	
			if (WeaponFragment->WeaponType != EquipmentInfo.RequiredWeaponType)
				return false;
		}
		else if (EquipmentInfo.EquipmentType == EEquipmentType::Utility)
		{
			UL1ItemInstance* ItemInstance = EquipManager->GetFirstEquippedItemInstance();
			if (ItemInstance == nullptr)
				return false;

			const UL1ItemFragment_Equipable_Utility* UtilityFragment = ItemInstance->FindFragmentByClass<UL1ItemFragment_Equipable_Utility>();
			if (UtilityFragment == nullptr)
				return false;

			if (UtilityFragment->UtilityType != EquipmentInfo.RequiredUtilityType)
				return false;
		}
	}

	return true;
}

AL1EquipmentBase* UL1GameplayAbility_Equipment::GetFirstEquipmentActor() const
{
	for (const FL1EquipmentInfo& EquipmentInfo : EquipmentInfos)
	{
		if (AL1EquipmentBase* EquipmentActor = EquipmentInfo.EquipmentActor.Get())
			return EquipmentActor;
	}
	return nullptr;
}

UL1ItemInstance* UL1GameplayAbility_Equipment::GetEquipmentItemInstance(const AL1EquipmentBase* InEquipmentActor) const
{
	if (InEquipmentActor == nullptr)
		return nullptr;
	
	UL1EquipManagerComponent* EquipManager = GetLyraCharacterFromActorInfo()->FindComponentByClass<UL1EquipManagerComponent>();
	if (EquipManager == nullptr)
		return nullptr;
	
	for (const FL1EquipmentInfo& EquipmentInfo : EquipmentInfos)
	{
		if (EquipmentInfo.EquipmentActor != InEquipmentActor)
			continue;
		
		return EquipManager->GetEquippedItemInstance(InEquipmentActor->GetEquipmentSlotType());
	}

	return nullptr;
}

int32 UL1GameplayAbility_Equipment::GetEquipmentStatValue(FGameplayTag InStatTag, const AL1EquipmentBase* InEquipmentActor) const
{
	if (InStatTag.IsValid() == false || InEquipmentActor == nullptr)
		return 0;
	
	if (UL1ItemInstance* ItemInstance = GetEquipmentItemInstance(InEquipmentActor))
		return ItemInstance->GetStackCountByTag(InStatTag);

	check(true);
	
	return 0;
}
