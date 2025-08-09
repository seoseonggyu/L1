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
#include "PocketWorld/L1PocketStage.h"
#include "PocketWorld/L1PocketWorldSubsystem.h"
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

		// Add Current Stat // SSG: 스탯은 아직 넣지는 않는다
		/*const TSubclassOf<UGameplayEffect> AttributeModifierGE = ULyraAssetManager::GetSubclassByPath(ULyraGameData::Get().AttributeModifierGameplayEffect);
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
		*/
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
	else
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
}

UL1EquipManagerComponent::UL1EquipManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
}

void UL1EquipManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
	if (GetOwner())
	{
		Entries.SetNum((int32)EEquipmentSlotType::Count);

		for (int32 i = 0; i < Entries.Num(); i++)
		{
			FL1EquipEntry& Entry = Entries[i];
			Entry.EquipManager = this;
			Entry.EquipmentSlotType = (EEquipmentSlotType)i;
		}
	}
}

void UL1EquipManagerComponent::UninitializeComponent()
{
	if (GetOwner())
	{
		for (int32 i = 0; i < (int32)EEquipmentSlotType::Count; i++)
		{
			Unequip((EEquipmentSlotType)i);
		}
	}

	Super::UninitializeComponent();
}

void UL1EquipManagerComponent::Equip(EEquipmentSlotType EquipmentSlotType, UL1ItemInstance* ItemInstance)
{
	if (EquipmentSlotType == EEquipmentSlotType::Count || ItemInstance == nullptr)
		return;

	FL1EquipEntry& Entry = Entries[(int32)(EquipmentSlotType)];
	Entry.Init(ItemInstance);
}

void UL1EquipManagerComponent::Unequip(EEquipmentSlotType EquipmentSlotType)
{
	if (EquipmentSlotType == EEquipmentSlotType::Count)
		return;

	FL1EquipEntry& Entry = Entries[(int32)EquipmentSlotType];
	UL1ItemInstance* RemovedItemInstance = Entry.GetItemInstance();
	Entry.Init(nullptr);
}

void UL1EquipManagerComponent::EquipCurrentSlots()
{
	if (CurrentEquipState == EEquipState::Count)
		return;

	if (UL1EquipmentManagerComponent* EquipmentManager = GetEquipmentManager())
	{
		for (EEquipmentSlotType EquipmentSlotType : UL1EquipManagerComponent::GetEquipmentSlotsByEquipState(CurrentEquipState))
		{
			Equip(EquipmentSlotType, EquipmentManager->GetItemInstance(EquipmentSlotType));
		}
	}
}

void UL1EquipManagerComponent::UnequipCurrentSlots()
{
	if (CurrentEquipState == EEquipState::Count)
		return;

	for (EEquipmentSlotType EquipmentSlotType : UL1EquipManagerComponent::GetEquipmentSlotsByEquipState(CurrentEquipState))
	{
		Unequip(EquipmentSlotType);
	}
}

void UL1EquipManagerComponent::ChangeEquipState(EEquipState NewEquipState)
{
	if (CanChangeEquipState(NewEquipState))
	{
		if (CurrentEquipState == NewEquipState)
		{
			NewEquipState = EEquipState::Unarmed;
		}

		UnequipCurrentSlots();
		CurrentEquipState = NewEquipState;
		EquipCurrentSlots();
	}
}

bool UL1EquipManagerComponent::CanChangeEquipState(EEquipState NewEquipState) const
{
	if (NewEquipState == EEquipState::Count)
		return false;

	if (CurrentEquipState == EEquipState::Unarmed && NewEquipState == EEquipState::Unarmed)
		return false;

	if (CurrentEquipState == NewEquipState)
		return true;

	UL1EquipmentManagerComponent* EquipmentManager = GetEquipmentManager();
	if (EquipmentManager == nullptr)
		return false;

	return (EquipmentManager->IsAllEmpty(NewEquipState) == false);
}

void UL1EquipManagerComponent::CurrentEquip(EEquipState PrevEquipState)
{
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

TArray<FL1EquipEntry>& UL1EquipManagerComponent::GetEntries()
{
	return Entries;
}

UAbilitySystemComponent* UL1EquipManagerComponent::GetAbilitySystemComponent() const
{
	return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
}

UL1EquipmentManagerComponent* UL1EquipManagerComponent::GetEquipmentManager() const
{
	if (ALyraCharacter* LyraCharacter = GetCharacter())
	{
		return LyraCharacter->FindComponentByClass<UL1EquipmentManagerComponent>();
	}
	return nullptr;
}

EEquipmentSlotType UL1EquipManagerComponent::ConvertToEquipmentSlotType(EWeaponHandType WeaponHandType, EEquipState EquipState)
{
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

	if (EquipState == EEquipState::Unarmed)
	{
		switch (WeaponHandType)
		{
		case EWeaponHandType::LeftHand:  EquipmentSlotType = EEquipmentSlotType::Unarmed_LeftHand;  break;
		case EWeaponHandType::RightHand: EquipmentSlotType = EEquipmentSlotType::Unarmed_RightHand; break;
		}
	}
	else if (EquipState == EEquipState::Weapon_Primary)
	{
		switch (WeaponHandType)
		{
		case EWeaponHandType::LeftHand:  EquipmentSlotType = EEquipmentSlotType::Primary_LeftHand;  break;
		case EWeaponHandType::RightHand: EquipmentSlotType = EEquipmentSlotType::Primary_RightHand; break;
		case EWeaponHandType::TwoHand:   EquipmentSlotType = EEquipmentSlotType::Primary_TwoHand;   break;
		}
	}
	else if (EquipState == EEquipState::Weapon_Secondary)
	{
		switch (WeaponHandType)
		{
		case EWeaponHandType::LeftHand:  EquipmentSlotType = EEquipmentSlotType::Secondary_LeftHand;  break;
		case EWeaponHandType::RightHand: EquipmentSlotType = EEquipmentSlotType::Secondary_RightHand; break;
		case EWeaponHandType::TwoHand:   EquipmentSlotType = EEquipmentSlotType::Secondary_TwoHand;   break;
		}
	}
	else if (EquipState == EEquipState::Utility_Primary)
	{
		EquipmentSlotType = EEquipmentSlotType::Utility_Primary;
	}
	else if (EquipState == EEquipState::Utility_Secondary)
	{
		EquipmentSlotType = EEquipmentSlotType::Utility_Secondary;
	}
	else if (EquipState == EEquipState::Utility_Tertiary)
	{
		EquipmentSlotType = EEquipmentSlotType::Utility_Tertiary;
	}
	else if (EquipState == EEquipState::Utility_Quaternary)
	{
		EquipmentSlotType = EEquipmentSlotType::Utility_Quaternary;
	}

	return EquipmentSlotType;
}

EEquipmentSlotType UL1EquipManagerComponent::ConvertToEquipmentSlotType(EWeaponHandType WeaponHandType, EWeaponSlotType WeaponSlotType)
{
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

	if (WeaponSlotType == EWeaponSlotType::Primary)
	{
		switch (WeaponHandType)
		{
		case EWeaponHandType::LeftHand:  EquipmentSlotType = EEquipmentSlotType::Primary_LeftHand;  break;
		case EWeaponHandType::RightHand: EquipmentSlotType = EEquipmentSlotType::Primary_RightHand; break;
		case EWeaponHandType::TwoHand:   EquipmentSlotType = EEquipmentSlotType::Primary_TwoHand;   break;
		}
	}
	else if (WeaponSlotType == EWeaponSlotType::Secondary)
	{
		switch (WeaponHandType)
		{
		case EWeaponHandType::LeftHand:  EquipmentSlotType = EEquipmentSlotType::Secondary_LeftHand;  break;
		case EWeaponHandType::RightHand: EquipmentSlotType = EEquipmentSlotType::Secondary_RightHand; break;
		case EWeaponHandType::TwoHand:   EquipmentSlotType = EEquipmentSlotType::Secondary_TwoHand;   break;
		}
	}

	return EquipmentSlotType;
}

EEquipmentSlotType UL1EquipManagerComponent::ConvertToEquipmentSlotType(EArmorType ArmorType)
{
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

	switch (ArmorType)
	{
	case EArmorType::Helmet: EquipmentSlotType = EEquipmentSlotType::Helmet; break;
	case EArmorType::Chest:  EquipmentSlotType = EEquipmentSlotType::Chest;  break;
	case EArmorType::Legs:   EquipmentSlotType = EEquipmentSlotType::Legs;   break;
	case EArmorType::Hands:  EquipmentSlotType = EEquipmentSlotType::Hands;  break;
	case EArmorType::Foot:   EquipmentSlotType = EEquipmentSlotType::Foot;   break;
	}

	return EquipmentSlotType;
}

EEquipmentSlotType UL1EquipManagerComponent::ConvertToEquipmentSlotType(EUtilitySlotType UtilitySlotType)
{
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

	switch (UtilitySlotType)
	{
	case EUtilitySlotType::Primary:		EquipmentSlotType = EEquipmentSlotType::Utility_Primary;	break;
	case EUtilitySlotType::Secondary:	EquipmentSlotType = EEquipmentSlotType::Utility_Secondary;	break;
	case EUtilitySlotType::Tertiary:	EquipmentSlotType = EEquipmentSlotType::Utility_Tertiary;	break;
	case EUtilitySlotType::Quaternary:	EquipmentSlotType = EEquipmentSlotType::Utility_Quaternary;	break;
	}

	return EquipmentSlotType;
}

EWeaponSlotType UL1EquipManagerComponent::ConvertToWeaponSlotType(EEquipmentSlotType EquipmentSlotType)
{
	EWeaponSlotType WeaponSlotType = EWeaponSlotType::Count;

	switch (EquipmentSlotType)
	{
	case EEquipmentSlotType::Primary_LeftHand:
	case EEquipmentSlotType::Primary_RightHand:
	case EEquipmentSlotType::Primary_TwoHand:
		WeaponSlotType = EWeaponSlotType::Primary;
		break;
	case EEquipmentSlotType::Secondary_LeftHand:
	case EEquipmentSlotType::Secondary_RightHand:
	case EEquipmentSlotType::Secondary_TwoHand:
		WeaponSlotType = EWeaponSlotType::Secondary;
		break;
	}

	return WeaponSlotType;
}

EWeaponHandType UL1EquipManagerComponent::ConvertToWeaponHandType(EEquipmentSlotType EquipmentSlotType)
{
	EWeaponHandType WeaponHandType = EWeaponHandType::Count;

	switch (EquipmentSlotType)
	{
	case EEquipmentSlotType::Unarmed_LeftHand:
	case EEquipmentSlotType::Primary_LeftHand:
	case EEquipmentSlotType::Secondary_LeftHand:
		WeaponHandType = EWeaponHandType::LeftHand;
		break;
	case EEquipmentSlotType::Unarmed_RightHand:
	case EEquipmentSlotType::Primary_RightHand:
	case EEquipmentSlotType::Secondary_RightHand:
		WeaponHandType = EWeaponHandType::RightHand;
		break;
	case EEquipmentSlotType::Primary_TwoHand:
	case EEquipmentSlotType::Secondary_TwoHand:
		WeaponHandType = EWeaponHandType::TwoHand;
		break;
	}

	return WeaponHandType;
}

EArmorType UL1EquipManagerComponent::ConvertToArmorType(EEquipmentSlotType EquipmentSlotType)
{
	EArmorType ArmorType = EArmorType::Count;

	switch (EquipmentSlotType)
	{
	case EEquipmentSlotType::Helmet:	ArmorType = EArmorType::Helmet;	break;
	case EEquipmentSlotType::Chest:		ArmorType = EArmorType::Chest;	break;
	case EEquipmentSlotType::Legs:		ArmorType = EArmorType::Legs;	break;
	case EEquipmentSlotType::Hands:		ArmorType = EArmorType::Hands;	break;
	case EEquipmentSlotType::Foot:		ArmorType = EArmorType::Foot;	break;
	}

	return ArmorType;
}

EUtilitySlotType UL1EquipManagerComponent::ConvertToUtilitySlotType(EEquipmentSlotType EquipmentSlotType)
{
	EUtilitySlotType UtilitySlotType = EUtilitySlotType::Count;

	switch (EquipmentSlotType)
	{
	case EEquipmentSlotType::Utility_Primary:		UtilitySlotType = EUtilitySlotType::Primary;	break;
	case EEquipmentSlotType::Utility_Secondary:		UtilitySlotType = EUtilitySlotType::Secondary;	break;
	case EEquipmentSlotType::Utility_Tertiary:		UtilitySlotType = EUtilitySlotType::Tertiary;	break;
	case EEquipmentSlotType::Utility_Quaternary:	UtilitySlotType = EUtilitySlotType::Quaternary;	break;
	}

	return UtilitySlotType;
}

EEquipState UL1EquipManagerComponent::ConvertToEquipState(EWeaponSlotType WeaponSlotType)
{
	EEquipState EquipState = EEquipState::Count;

	switch (WeaponSlotType)
	{
	case EWeaponSlotType::Primary:		EquipState = EEquipState::Weapon_Primary;		break;
	case EWeaponSlotType::Secondary:	EquipState = EEquipState::Weapon_Secondary;		break;
	}

	return EquipState;
}

EEquipState UL1EquipManagerComponent::ConvertToEquipState(EUtilitySlotType UtilitySlotType)
{
	EEquipState EquipState = EEquipState::Count;

	switch (UtilitySlotType)
	{
	case EUtilitySlotType::Primary:		EquipState = EEquipState::Utility_Primary;		break;
	case EUtilitySlotType::Secondary:	EquipState = EEquipState::Utility_Secondary;	break;
	case EUtilitySlotType::Tertiary:	EquipState = EEquipState::Utility_Tertiary;		break;
	case EUtilitySlotType::Quaternary:	EquipState = EEquipState::Utility_Quaternary;	break;
	}

	return EquipState;
}

bool UL1EquipManagerComponent::IsWeaponEquipState(EEquipState EquipState)
{
	return (EEquipState::Unarmed <= EquipState && EquipState <= EEquipState::Weapon_Secondary);
}

bool UL1EquipManagerComponent::IsUtilityEquipState(EEquipState EquipState)
{
	return (EEquipState::Utility_Primary <= EquipState && EquipState <= EEquipState::Utility_Quaternary);
}

const TArray<EEquipmentSlotType>& UL1EquipManagerComponent::GetEquipmentSlotsByEquipState(EEquipState EquipState)
{
	static const TArray<TArray<EEquipmentSlotType>> EquipmentSlotsByEquipState = {
		{ EEquipmentSlotType::Unarmed_LeftHand,    EEquipmentSlotType::Unarmed_RightHand                                            },
		{ EEquipmentSlotType::Primary_LeftHand,    EEquipmentSlotType::Primary_RightHand,    EEquipmentSlotType::Primary_TwoHand    },
		{ EEquipmentSlotType::Secondary_LeftHand,  EEquipmentSlotType::Secondary_RightHand,  EEquipmentSlotType::Secondary_TwoHand  },
		{ EEquipmentSlotType::Utility_Primary }, { EEquipmentSlotType::Utility_Secondary }, { EEquipmentSlotType::Utility_Tertiary }, { EEquipmentSlotType::Utility_Quaternary },
	};

	if (EquipmentSlotsByEquipState.IsValidIndex((int32)EquipState))
	{
		return EquipmentSlotsByEquipState[(int32)EquipState];
	}
	else
	{
		static const TArray<EEquipmentSlotType> EmptyEquipmentSlots;
		return EmptyEquipmentSlots;
	}
}

void UL1EquipManagerComponent::ChangeShouldHiddenEquipments(bool bNewShouldHiddenEquipments)
{
	bShouldHiddenEquipments = bNewShouldHiddenEquipments;

	TArray<AL1EquipmentBase*> OutEquippedActors;
	GetAllEquippedActors(OutEquippedActors);

	for (AL1EquipmentBase* WeaponActor : OutEquippedActors)
	{
		if (IsValid(WeaponActor))
		{
			WeaponActor->SetActorHiddenInGame(bShouldHiddenEquipments);
		}
	}
}

AL1EquipmentBase* UL1EquipManagerComponent::GetFirstEquippedActor() const
{
	AL1EquipmentBase* EquipmentActor = nullptr;

	if (IsWeaponEquipState(CurrentEquipState))
	{
		for (int i = 0; i < (int32)EWeaponHandType::Count; i++)
		{
			const int32 EntryIndex = (int32)ConvertToEquipmentSlotType((EWeaponHandType)i, CurrentEquipState);
			if (Entries.IsValidIndex(EntryIndex) == false)
				continue;

			EquipmentActor = Entries[EntryIndex].GetEquipmentActor();
			if (EquipmentActor)
				break;
		}
	}
	else if (IsUtilityEquipState(CurrentEquipState))
	{
		const int32 EntryIndex = (int32)ConvertToEquipmentSlotType(EWeaponHandType::Count, CurrentEquipState);
		if (Entries.IsValidIndex(EntryIndex))
		{
			EquipmentActor = Entries[EntryIndex].GetEquipmentActor();
		}
	}

	return EquipmentActor;
}

AL1EquipmentBase* UL1EquipManagerComponent::GetEquippedActor(EWeaponHandType WeaponHandType) const
{
	if (WeaponHandType == EWeaponHandType::Count)
		return GetFirstEquippedActor();

	const int32 EntryIndex = (int32)ConvertToEquipmentSlotType(WeaponHandType, CurrentEquipState);
	return Entries.IsValidIndex(EntryIndex) ? Entries[EntryIndex].GetEquipmentActor() : nullptr;
}

void UL1EquipManagerComponent::GetAllEquippedActors(TArray<AL1EquipmentBase*>& OutActors) const
{
	OutActors.Reset();

	if (IsWeaponEquipState(CurrentEquipState))
	{
		for (int32 i = 0; i < (int32)EWeaponHandType::Count; i++)
		{
			const int32 EntryIndex = (int32)ConvertToEquipmentSlotType((EWeaponHandType)i, CurrentEquipState);
			if (Entries.IsValidIndex(EntryIndex) && Entries[EntryIndex].GetEquipmentActor())
			{
				OutActors.Add(Entries[EntryIndex].GetEquipmentActor());
			}
		}
	}
	else if (IsUtilityEquipState(CurrentEquipState))
	{
		const int32 EntryIndex = (int32)ConvertToEquipmentSlotType(EWeaponHandType::Count, CurrentEquipState);
		if (Entries.IsValidIndex(EntryIndex) && Entries[EntryIndex].GetEquipmentActor())
		{
			OutActors.Add(Entries[EntryIndex].GetEquipmentActor());
		}
	}
}

UL1ItemInstance* UL1EquipManagerComponent::GetFirstEquippedItemInstance(bool bIgnoreArmor) const
{
	if (bIgnoreArmor == false)
	{
		for (int i = 0; i < (int32)EArmorType::Count; i++)
		{
			const int32 EntryIndex = (int32)ConvertToEquipmentSlotType((EArmorType)i);
			if (Entries.IsValidIndex(EntryIndex) == false)
				continue;

			if (UL1ItemInstance* ItemInstance = Entries[EntryIndex].GetItemInstance())
				return ItemInstance;
		}
	}

	UL1ItemInstance* ItemInstance = nullptr;

	if (IsWeaponEquipState(CurrentEquipState))
	{
		for (int i = 0; i < (int32)EWeaponHandType::Count; i++)
		{
			const int32 EntryIndex = (int32)ConvertToEquipmentSlotType((EWeaponHandType)i, CurrentEquipState);
			if (Entries.IsValidIndex(EntryIndex) == false)
				continue;

			ItemInstance = Entries[EntryIndex].GetItemInstance();
			if (ItemInstance)
				break;
		}
	}
	else if (IsUtilityEquipState(CurrentEquipState))
	{
		const int32 EntryIndex = (int32)ConvertToEquipmentSlotType(EWeaponHandType::Count, CurrentEquipState);
		if (Entries.IsValidIndex(EntryIndex))
		{
			ItemInstance = Entries[EntryIndex].GetItemInstance();
		}
	}

	return ItemInstance;
}

UL1ItemInstance* UL1EquipManagerComponent::GetEquippedItemInstance(EArmorType ArmorType) const
{
	const int32 EntryIndex = (int32)ConvertToEquipmentSlotType(ArmorType);
	return Entries.IsValidIndex(EntryIndex) ? Entries[EntryIndex].GetItemInstance() : nullptr;
}

UL1ItemInstance* UL1EquipManagerComponent::GetEquippedItemInstance(EWeaponHandType WeaponHandType) const
{
	if (WeaponHandType == EWeaponHandType::Count)
		return GetFirstEquippedItemInstance();

	const int32 EntryIndex = (int32)ConvertToEquipmentSlotType(WeaponHandType, CurrentEquipState);
	return Entries.IsValidIndex(EntryIndex) ? Entries[EntryIndex].GetItemInstance() : nullptr;
}

UL1ItemInstance* UL1EquipManagerComponent::GetEquippedItemInstance(EEquipmentSlotType EquipmentSlotType) const
{
	if (EquipmentSlotType == EEquipmentSlotType::Count)
		return nullptr;

	const int32 EntryIndex = (int32)EquipmentSlotType;
	return Entries.IsValidIndex(EntryIndex) ? Entries[EntryIndex].GetItemInstance() : nullptr;
}
