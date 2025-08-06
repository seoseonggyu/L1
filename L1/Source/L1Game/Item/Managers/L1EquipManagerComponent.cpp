#include "L1EquipManagerComponent.h"

#include "AbilitySystemGlobals.h"
#include "L1CosmeticManagerComponent.h"
#include "L1EquipmentManagerComponent.h"
#include "L1GameplayTags.h"
#include "Engine/ActorChannel.h"
#include "Item/L1ItemInstance.h"
#include "Item/Fragments/L1ItemFragment_Equipable.h"
#include "Item/Fragments/L1ItemFragment_Equipable_Armor.h"
#include "Item/Fragments/L1ItemFragment_Equipable_Weapon.h"
#include "Net/UnrealNetwork.h"
#include "Actors/L1EquipmentBase.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
// #include "AbilitySystem/Attributes/L1CombatSet.h" // SSG: 
#include "Character/LyraCharacter.h"
#include "Player/LyraPlayerController.h"
//#include "PocketWorld/L1PocketStage.h"			// SSG: 
//#include "PocketWorld/L1PocketWorldSubsystem.h"	// SSG: 
#include "System/L1GameplayTagStack.h"
#include "System/LyraAssetManager.h"
#include "System/LyraGameData.h"

void FL1EquipEntry::Init(UL1ItemInstance* InItemInstance)
{
	if (ItemInstance == InItemInstance)
		return;

	ItemInstance = InItemInstance;
	ItemInstance ? Equip() : Unequip();
}

void FL1EquipEntry::Equip()
{
	if (ItemInstance == nullptr)
		return;

	ALyraCharacter* Character = EquipManager->GetCharacter();
	if (Character == nullptr)
		return;

	const UL1ItemFragment_Equipable* EquippableFragment = ItemInstance->FindFragmentByClass<UL1ItemFragment_Equipable>();
	if (EquippableFragment == nullptr)
		return;

	if (ULyraAbilitySystemComponent* ASC = Cast<ULyraAbilitySystemComponent>(EquipManager->GetAbilitySystemComponent()))
	{
		// Remove Previous Ability
		BaseAbilitySetHandles.TakeFromAbilitySystem(ASC);

		// Add Current Ability
		if (const ULyraAbilitySet* BaseAbilitySet = EquippableFragment->BaseAbilitySet)
		{
			BaseAbilitySet->GiveToAbilitySystem(ASC, &BaseAbilitySetHandles, ItemInstance);
		}

		// Remove Previous Stat
		ASC->RemoveActiveGameplayEffect(BaseStatHandle);
		BaseStatHandle.Invalidate();

		// Add Current Stat
		const TSubclassOf<UGameplayEffect> AttributeModifierGE = ULyraAssetManager::GetSubclassByPath(ULyraGameData::Get().AttributeModifierGameplayEffect);
		check(AttributeModifierGE);

		const FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(AttributeModifierGE, 1.f, ContextHandle);
		const TSharedPtr<FGameplayEffectSpec>& SpecData = SpecHandle.Data;

		for (const FGameplayModifierInfo& ModifierInfo : SpecData->Def->Modifiers)
		{
			SpecData->SetSetByCallerMagnitude(ModifierInfo.ModifierMagnitude.GetSetByCallerFloat().DataTag, 0);
		}

		for (const FL1GameplayTagStack& Stack : ItemInstance->GetStatContainer().GetStacks())
		{
			SpecData->SetSetByCallerMagnitude(Stack.GetStackTag(), Stack.GetStackCount());
		}

		BaseStatHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	}

	if (EquippableFragment->EquipmentType == EEquipmentType::Weapon || EquippableFragment->EquipmentType == EEquipmentType::Utility)
	{
		// Despawn Previous Real Weapon
		if (IsValid(SpawnedEquipmentActor))
		{
			SpawnedEquipmentActor->Destroy();
		}

		// Spawn Current Real Weapon
		const UL1ItemFragment_Equipable_Attachment* AttachmentFragment = ItemInstance->FindFragmentByClass<UL1ItemFragment_Equipable_Attachment>();
		const FL1WeaponAttachInfo& AttachInfo = AttachmentFragment->WeaponAttachInfo;
		if (AttachInfo.SpawnWeaponClass)
		{
			UWorld* World = EquipManager->GetWorld();
			AL1EquipmentBase* NewWeaponActor = World->SpawnActorDeferred<AL1EquipmentBase>(AttachInfo.SpawnWeaponClass, FTransform::Identity, Character);
			NewWeaponActor->Init(ItemInstance->GetItemTemplateID(), EquipmentSlotType);
			NewWeaponActor->SetActorRelativeTransform(AttachInfo.AttachTransform);
			NewWeaponActor->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, AttachInfo.AttachSocket);
			NewWeaponActor->SetActorHiddenInGame(EquipManager->ShouldHiddenEquipments());
			NewWeaponActor->FinishSpawning(FTransform::Identity, true);
		}
	}
	/*else
		// SSG: 포켓 월드
	
	{
		if (EquippableFragment->EquipmentType == EEquipmentType::Weapon || EquippableFragment->EquipmentType == EEquipmentType::Utility)
		{
			if (Character->IsLocallyControlled())
			{
				// Despawn Previous Pocket Weapon
				if (IsValid(SpawnedPocketWorldActor))
				{
					SpawnedPocketWorldActor->Destroy();
				}

				// Spawn Current Pocket Weapon
				const UL1ItemFragment_Equipable_Attachment* AttachmentFragment = ItemInstance->FindFragmentByClass<UL1ItemFragment_Equipable_Attachment>();
				if (UL1PocketWorldSubsystem* PocketWorldSubsystem = EquipManager->GetWorld()->GetSubsystem<UL1PocketWorldSubsystem>())
				{
					if (APlayerController* PC = Character->GetLyraPlayerController())
					{
						PocketWorldSubsystem->RegisterAndCallForGetPocketStage(PC->GetLocalPlayer(),
							FGetPocketStageDelegate::CreateLambda([this, AttachmentFragment](AL1PocketStage* PocketStage)
								{
									if (IsValid(PocketStage))
									{
										ACharacter* Character = PocketStage->GetCharacter();
										const FL1WeaponAttachInfo& AttachInfo = AttachmentFragment->WeaponAttachInfo;

										UWorld* World = EquipManager->GetWorld();
										SpawnedPocketWorldActor = World->SpawnActorDeferred<AL1EquipmentBase>(AttachInfo.SpawnWeaponClass, FTransform::Identity, Character);
										SpawnedPocketWorldActor->SetActorRelativeTransform(AttachInfo.AttachTransform);
										SpawnedPocketWorldActor->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, AttachInfo.AttachSocket);
										SpawnedPocketWorldActor->bOnlyUseForLocal = true;
										SpawnedPocketWorldActor->FinishSpawning(FTransform::Identity, true);

										PocketStage->RefreshLightingChannelToActors();

										UAnimMontage* PocketWorldIdleMontage = ULyraAssetManager::GetAssetByPath<UAnimMontage>(AttachmentFragment->PocketWorldIdleMontage);
										Character->PlayAnimMontage(PocketWorldIdleMontage);
									}
								})
						);
					}
				}
			}
		}
		else if (EquippableFragment->EquipmentType == EEquipmentType::Armor)
		{
			// Refresh Real Armor Mesh
			const UL1ItemFragment_Equipable_Armor* ArmorFragment = ItemInstance->FindFragmentByClass<UL1ItemFragment_Equipable_Armor>();
			if (UL1CosmeticManagerComponent* CharacterCosmetics = Character->FindComponentByClass<UL1CosmeticManagerComponent>())
			{
				CharacterCosmetics->RefreshArmorMesh(ArmorFragment->ArmorType, ArmorFragment);
			}

			// Refresh Pocket Armor Mesh
			if (Character->IsLocallyControlled())
			{
				if (UL1PocketWorldSubsystem* PocketWorldSubsystem = EquipManager->GetWorld()->GetSubsystem<UL1PocketWorldSubsystem>())
				{
					if (APlayerController* PC = Character->GetLyraPlayerController())
					{
						PocketWorldSubsystem->RegisterAndCallForGetPocketStage(PC->GetLocalPlayer(),
							FGetPocketStageDelegate::CreateLambda([ArmorFragment](AL1PocketStage* PocketStage)
								{
									if (IsValid(PocketStage))
									{
										if (UL1CosmeticManagerComponent* CosmeticManager = PocketStage->GetCosmeticManager())
										{
											CosmeticManager->RefreshArmorMesh(ArmorFragment->ArmorType, ArmorFragment);
										}
									}
								})
						);
					}
				}
			}
		}
	}*/

	if (EquippableFragment->EquipmentType == EEquipmentType::Weapon || EquippableFragment->EquipmentType == EEquipmentType::Utility)
	{
		const UL1ItemFragment_Equipable_Attachment* AttachmentFragment = ItemInstance->FindFragmentByClass<UL1ItemFragment_Equipable_Attachment>();
		if (USkeletalMeshComponent* MeshComponent = Character->GetMesh())
		{
			if (AttachmentFragment->AnimInstanceClass)
			{
				MeshComponent->LinkAnimClassLayers(AttachmentFragment->AnimInstanceClass);
			}

			UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
			if (ASC && ASC->HasMatchingGameplayTag(L1GameplayTags::Status_Interact) == false)
			{
				UAnimMontage* EquipMontage = ULyraAssetManager::GetAssetByPath<UAnimMontage>(AttachmentFragment->EquipMontage);
				if (UAnimInstance* AnimInstance = MeshComponent->GetAnimInstance())
				{
					if (AnimInstance->GetCurrentActiveMontage() != EquipMontage)
					{
						Character->PlayAnimMontage(EquipMontage);
					}
				}
			}
		}
	}
}

void FL1EquipEntry::Unequip()
{
	if (ULyraAbilitySystemComponent* ASC = Cast<ULyraAbilitySystemComponent>(EquipManager->GetAbilitySystemComponent()))
	{
		// Remove Ability
		BaseAbilitySetHandles.TakeFromAbilitySystem(ASC);

		// Remove Stat
		ASC->RemoveActiveGameplayEffect(BaseStatHandle);
		BaseStatHandle.Invalidate();
	}

	// Despawn Real Weapon
	if (UL1EquipmentManagerComponent::IsWeaponSlot(EquipmentSlotType) || UL1EquipmentManagerComponent::IsUtilitySlot(EquipmentSlotType))
	{
		if (IsValid(SpawnedEquipmentActor))
		{
			SpawnedEquipmentActor->Destroy();
		}
	}
	// SSG: 포켓 월드
	/*else
	{
		if (ALyraCharacter* Character = EquipManager->GetCharacter())
		{
			if (UL1EquipmentManagerComponent::IsWeaponSlot(EquipmentSlotType) || UL1EquipmentManagerComponent::IsUtilitySlot(EquipmentSlotType))
			{
				// Despawn Pocket Weapon
				if (Character->IsLocallyControlled())
				{
					if (UL1PocketWorldSubsystem* PocketWorldSubsystem = EquipManager->GetWorld()->GetSubsystem<UL1PocketWorldSubsystem>())
					{
						if (APlayerController* PC = Character->GetLyraPlayerController())
						{
							PocketWorldSubsystem->RegisterAndCallForGetPocketStage(PC->GetLocalPlayer(),
								FGetPocketStageDelegate::CreateLambda([this](AL1PocketStage* PocketStage)
									{
										if (IsValid(PocketStage))
										{
											if (IsValid(SpawnedPocketWorldActor))
											{
												SpawnedPocketWorldActor->Destroy();
											}
										}
									})
							);
						}
					}
				}
			}
			else if (UL1EquipmentManagerComponent::IsArmorSlot(EquipmentSlotType))
			{
				// Refresh Real Armor Mesh
				EArmorType ArmorType = EquipManager->ConvertToArmorType(EquipmentSlotType);

				if (UL1CosmeticManagerComponent* CharacterCosmetics = Character->FindComponentByClass<UL1CosmeticManagerComponent>())
				{
					CharacterCosmetics->RefreshArmorMesh(ArmorType, nullptr);
				}

				// Refresh Pocket Armor Mesh
				if (Character->IsLocallyControlled())
				{
					if (UL1PocketWorldSubsystem* PocketWorldSubsystem = EquipManager->GetWorld()->GetSubsystem<UL1PocketWorldSubsystem>())
					{
						if (APlayerController* PC = Character->GetLyraPlayerController())
						{
							PocketWorldSubsystem->RegisterAndCallForGetPocketStage(PC->GetLocalPlayer(),
								FGetPocketStageDelegate::CreateLambda([ArmorType](AL1PocketStage* PocketStage)
									{
										if (IsValid(PocketStage))
										{
											if (UL1CosmeticManagerComponent* CosmeticManager = PocketStage->GetCosmeticManager())
											{
												CosmeticManager->RefreshArmorMesh(ArmorType, nullptr);
											}
										}
									})
							);
						}
					}
				}
			}
		}
	}
	*/
}

ALyraCharacter* UL1EquipManagerComponent::GetCharacter() const
{
	return Cast<ALyraCharacter>(GetOwner());
}

ALyraPlayerController* UL1EquipManagerComponent::GetPlayerController() const
{
	if (ALyraCharacter* LyraCharacter = GetCharacter())
	{
		return LyraCharacter->GetLyraPlayerController();
	}
	return nullptr;
}

UAbilitySystemComponent* UL1EquipManagerComponent::GetAbilitySystemComponent() const
{
	return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
}
