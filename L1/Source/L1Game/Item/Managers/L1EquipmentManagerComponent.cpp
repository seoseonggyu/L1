#include "L1EquipmentManagerComponent.h"

#include "Character/LyraCharacter.h"
#include "Item/Managers/L1EquipManagerComponent.h"
#include "Item/Fragments/L1ItemFragment_Equipable_Weapon.h"
#include "Item/Fragments/L1ItemFragment_Equipable_Armor.h"
#include "Item/L1ItemInstance.h"
#include "Item/L1ItemTemplate.h"
#include "Data/L1ItemData.h"

void FL1EquipmentEntry::Init(UL1ItemInstance* InItemInstance, int32 InItemCount)
{

	check(InItemInstance && InItemCount > 0);

	UL1EquipManagerComponent* EquipManager = EquipmentManager->GetEquipManager();
	if (EquipManager == nullptr)
		return;

	const UL1ItemFragment_Equipable* EquippableFragment = InItemInstance->FindFragmentByClass<UL1ItemFragment_Equipable>();
	if (EquippableFragment == nullptr)
		return;

	if (ItemInstance)
	{
		EquipManager->Unequip(EquipmentSlotType);
	}

	ItemInstance = InItemInstance;

	const UL1ItemTemplate& ItemTemplate = UL1ItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
	ItemCount = FMath::Clamp(InItemCount, 1, ItemTemplate.MaxStackCount);

	if (EquippableFragment->EquipmentType == EEquipmentType::Armor || EquipmentManager->IsSameEquipState(EquipmentSlotType, EquipManager->GetCurrentEquipState()))
	{
		EquipManager->Equip(EquipmentSlotType, ItemInstance);
	}
}

UL1ItemInstance* FL1EquipmentEntry::Reset()
{
	UL1EquipManagerComponent* EquipManager = EquipmentManager->GetEquipManager();
	if (EquipManager == nullptr)
		return nullptr;

	if (ItemInstance)
	{
		EquipManager->Unequip(EquipmentSlotType);
	}

	UL1ItemInstance* RemovedItemInstance = ItemInstance;
	ItemInstance = nullptr;
	ItemCount = 0;
	
	if (EquipmentManager->IsAllEmpty(EquipManager->GetCurrentEquipState()))
	{
		EquipManager->ChangeEquipState(EEquipState::Unarmed);
	}

	return RemovedItemInstance;
}

UL1EquipmentManagerComponent::UL1EquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
}

void UL1EquipmentManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (GetOwner())
	{
		Entries.SetNum((int32)EEquipmentSlotType::Count);

		for (int32 i = 0; i < Entries.Num(); i++)
		{
			FL1EquipmentEntry& Entry = Entries[i];
			Entry.EquipmentManager = this;
			Entry.EquipmentSlotType = (EEquipmentSlotType)i;
		}
	}
}

void UL1EquipmentManagerComponent::AddEquipment_Unsafe(EEquipmentSlotType EquipmentSlotType, UL1ItemInstance* ItemInstance, int32 ItemCount)
{
	if (EquipmentSlotType == EEquipmentSlotType::Count || ItemCount <= 0)
		return;

	FL1EquipmentEntry& Entry = Entries[(int32)EquipmentSlotType];

	if (Entry.GetItemInstance())
	{
		Entry.ItemCount += ItemCount;
	}
	else
	{
		if (ItemInstance == nullptr)
			return;

		Entry.Init(ItemInstance, ItemCount);
	}
}

UL1ItemInstance* UL1EquipmentManagerComponent::RemoveEquipment_Unsafe(EEquipmentSlotType EquipmentSlotType, int32 ItemCount)
{
	FL1EquipmentEntry& Entry = Entries[(int32)EquipmentSlotType];
	UL1ItemInstance* ItemInstance = Entry.GetItemInstance();

	Entry.ItemCount -= ItemCount;
	if (Entry.GetItemCount() <= 0)
	{
		UL1ItemInstance* RemovedItemInstance = Entry.Reset();
	}
	return ItemInstance;
}

void UL1EquipmentManagerComponent::SetEquipment(EEquipmentSlotType EquipmentSlotType, TSubclassOf<UL1ItemTemplate> ItemTemplateClass, EItemRarity ItemRarity, int32 ItemCount)
{
	if (EquipmentSlotType == EEquipmentSlotType::Count || ItemTemplateClass == nullptr || ItemRarity == EItemRarity::Count || ItemCount <= 0)
		return;

	const int32 ItemTemplateID = UL1ItemData::Get().FindItemTemplateIDByClass(ItemTemplateClass);
	const UL1ItemTemplate& ItemTemplate = UL1ItemData::Get().FindItemTemplateByID(ItemTemplateID);

	ItemCount = FMath::Clamp(ItemCount, 1, ItemTemplate.MaxStackCount);

	const UL1ItemFragment_Equipable* EquippableFragment = ItemTemplate.FindFragmentByClass<UL1ItemFragment_Equipable>();
	if (EquippableFragment == nullptr)
		return;

	FL1EquipmentEntry& Entry = Entries[(int32)EquipmentSlotType];
	Entry.Reset();

	if (EquippableFragment->EquipmentType == EEquipmentType::Weapon)
	{
		const UL1ItemFragment_Equipable_Weapon* WeaponFragment = Cast<UL1ItemFragment_Equipable_Weapon>(EquippableFragment);
		EWeaponHandType WeaponHandType = WeaponFragment->WeaponHandType;

		if (IsPrimaryWeaponSlot(EquipmentSlotType))
		{
			if (WeaponHandType == EWeaponHandType::LeftHand || WeaponHandType == EWeaponHandType::RightHand)
			{
				RemoveEquipment_Unsafe(EEquipmentSlotType::Primary_TwoHand, 1);
			}
			else if (WeaponHandType == EWeaponHandType::TwoHand)
			{
				RemoveEquipment_Unsafe(EEquipmentSlotType::Primary_LeftHand, 1);
				RemoveEquipment_Unsafe(EEquipmentSlotType::Primary_RightHand, 1);
			}
		}
		else if (IsSecondaryWeaponSlot(EquipmentSlotType))
		{
			if (WeaponHandType == EWeaponHandType::LeftHand || WeaponHandType == EWeaponHandType::RightHand)
			{
				RemoveEquipment_Unsafe(EEquipmentSlotType::Secondary_TwoHand, 1);
			}
			else if (WeaponHandType == EWeaponHandType::TwoHand)
			{
				RemoveEquipment_Unsafe(EEquipmentSlotType::Secondary_LeftHand, 1);
				RemoveEquipment_Unsafe(EEquipmentSlotType::Secondary_RightHand, 1);
			}
		}
	}

	UL1ItemInstance* AddedItemInstance = NewObject<UL1ItemInstance>();
	AddedItemInstance->Init(ItemTemplateID, ItemRarity);
	Entry.Init(AddedItemInstance, ItemCount);

	if (IsWeaponSlot(EquipmentSlotType))
	{
		if (UL1EquipManagerComponent* EquipManager = GetEquipManager())
		{
			EWeaponSlotType WeaponSlotType = UL1EquipManagerComponent::ConvertToWeaponSlotType(EquipmentSlotType);
			EEquipState EquipState = UL1EquipManagerComponent::ConvertToEquipState(WeaponSlotType);
			if (EquipManager->GetCurrentEquipState() != EquipState)
			{
				EquipManager->ChangeEquipState(EquipState);
			}
		}
	}
}

bool UL1EquipmentManagerComponent::IsWeaponSlot(EEquipmentSlotType EquipmentSlotType)
{
	return (EEquipmentSlotType::Unarmed_LeftHand <= EquipmentSlotType && EquipmentSlotType <= EEquipmentSlotType::Secondary_TwoHand);
}

bool UL1EquipmentManagerComponent::IsArmorSlot(EEquipmentSlotType EquipmentSlotType)
{
	return (EEquipmentSlotType::Helmet <= EquipmentSlotType && EquipmentSlotType <= EEquipmentSlotType::Foot);
}

bool UL1EquipmentManagerComponent::IsUtilitySlot(EEquipmentSlotType EquipmentSlotType)
{
	return (EEquipmentSlotType::Utility_Primary <= EquipmentSlotType && EquipmentSlotType <= EEquipmentSlotType::Utility_Quaternary);
}

bool UL1EquipmentManagerComponent::IsSameEquipState(EEquipmentSlotType EquipmentSlotType, EEquipState WeaponEquipState)
{
	return (((EquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand || EquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand) && WeaponEquipState == EEquipState::Unarmed) ||
		((EquipmentSlotType == EEquipmentSlotType::Primary_LeftHand || EquipmentSlotType == EEquipmentSlotType::Primary_RightHand || EquipmentSlotType == EEquipmentSlotType::Primary_TwoHand) && WeaponEquipState == EEquipState::Weapon_Primary) ||
		((EquipmentSlotType == EEquipmentSlotType::Secondary_LeftHand || EquipmentSlotType == EEquipmentSlotType::Secondary_RightHand || EquipmentSlotType == EEquipmentSlotType::Secondary_TwoHand) && WeaponEquipState == EEquipState::Weapon_Secondary) ||
		(EquipmentSlotType == EEquipmentSlotType::Utility_Primary && WeaponEquipState == EEquipState::Utility_Primary) ||
		(EquipmentSlotType == EEquipmentSlotType::Utility_Secondary && WeaponEquipState == EEquipState::Utility_Secondary) ||
		(EquipmentSlotType == EEquipmentSlotType::Utility_Tertiary && WeaponEquipState == EEquipState::Utility_Tertiary) ||
		(EquipmentSlotType == EEquipmentSlotType::Utility_Quaternary && WeaponEquipState == EEquipState::Utility_Quaternary));
}

bool UL1EquipmentManagerComponent::IsSameWeaponHandType(EEquipmentSlotType EquipmentSlotType, EWeaponHandType WeaponHandType)
{
	return (((EquipmentSlotType == EEquipmentSlotType::Primary_LeftHand || EquipmentSlotType == EEquipmentSlotType::Secondary_LeftHand) && WeaponHandType == EWeaponHandType::LeftHand) ||
		((EquipmentSlotType == EEquipmentSlotType::Primary_RightHand || EquipmentSlotType == EEquipmentSlotType::Secondary_RightHand) && WeaponHandType == EWeaponHandType::RightHand) ||
		((EquipmentSlotType == EEquipmentSlotType::Primary_TwoHand || EquipmentSlotType == EEquipmentSlotType::Secondary_TwoHand) && WeaponHandType == EWeaponHandType::TwoHand));

}

bool UL1EquipmentManagerComponent::IsSameArmorType(EEquipmentSlotType EquipmentSlotType, EArmorType ArmorType)
{
	return ((EquipmentSlotType == EEquipmentSlotType::Helmet && ArmorType == EArmorType::Helmet) || (EquipmentSlotType == EEquipmentSlotType::Chest && ArmorType == EArmorType::Chest) ||
		(EquipmentSlotType == EEquipmentSlotType::Legs && ArmorType == EArmorType::Legs) || (EquipmentSlotType == EEquipmentSlotType::Hands && ArmorType == EArmorType::Hands) ||
		(EquipmentSlotType == EEquipmentSlotType::Foot && ArmorType == EArmorType::Foot));
}

bool UL1EquipmentManagerComponent::IsPrimaryWeaponSlot(EEquipmentSlotType EquipmentSlotType)
{
	return (EEquipmentSlotType::Primary_LeftHand <= EquipmentSlotType && EquipmentSlotType <= EEquipmentSlotType::Primary_TwoHand);
}

bool UL1EquipmentManagerComponent::IsSecondaryWeaponSlot(EEquipmentSlotType EquipmentSlotType)
{
	return (EEquipmentSlotType::Secondary_LeftHand <= EquipmentSlotType && EquipmentSlotType <= EEquipmentSlotType::Secondary_TwoHand);
}

const UL1ItemInstance* UL1EquipmentManagerComponent::FindPairItemInstance(const UL1ItemInstance* InBaseItemInstance, EEquipmentSlotType& OutEquipmentSlotType) const
{
	if (InBaseItemInstance == nullptr)
		return nullptr;

	const UL1ItemFragment_Equipable* BaseEquippableFragment = InBaseItemInstance->FindFragmentByClass<UL1ItemFragment_Equipable>();
	if (BaseEquippableFragment == nullptr)
		return nullptr;

	UL1ItemInstance* SelectedItemInstance = nullptr;

	if (BaseEquippableFragment->EquipmentType == EEquipmentType::Weapon)
	{
		const UL1ItemFragment_Equipable_Weapon* BaseWeaponFragment = Cast<UL1ItemFragment_Equipable_Weapon>(BaseEquippableFragment);

		for (int32 i = 0; i < (int32)EEquipmentSlotType::Count; i++)
		{
			if (IsWeaponSlot((EEquipmentSlotType)i) == false)
				continue;

			const FL1EquipmentEntry& Entry = Entries[i];
			if (UL1ItemInstance* ItemInstance = Entry.ItemInstance)
			{
				if (const UL1ItemFragment_Equipable_Weapon* EquippedWeaponFragment = ItemInstance->FindFragmentByClass<UL1ItemFragment_Equipable_Weapon>())
				{
					if ((BaseWeaponFragment->WeaponType == EquippedWeaponFragment->WeaponType) && (BaseWeaponFragment->WeaponHandType == EquippedWeaponFragment->WeaponHandType))
					{
						if (InBaseItemInstance == ItemInstance)
						{
							SelectedItemInstance = nullptr;
							break;
						}

						if (SelectedItemInstance == nullptr)
						{
							SelectedItemInstance = ItemInstance;
							OutEquipmentSlotType = (EEquipmentSlotType)i;
						}
					}
				}
			}
		}
	}
	else if (BaseEquippableFragment->EquipmentType == EEquipmentType::Armor)
	{
		const UL1ItemFragment_Equipable_Armor* BaseArmorFragment = Cast<UL1ItemFragment_Equipable_Armor>(BaseEquippableFragment);
		OutEquipmentSlotType = UL1EquipManagerComponent::ConvertToEquipmentSlotType(BaseArmorFragment->ArmorType); 
		SelectedItemInstance = GetItemInstance(OutEquipmentSlotType);
	}

	if (InBaseItemInstance == SelectedItemInstance)
	{
		SelectedItemInstance = nullptr;
	}

	return SelectedItemInstance;
}

bool UL1EquipmentManagerComponent::IsAllEmpty(EEquipState EquipState) const
{
	if (EquipState == EEquipState::Count)
		return true;

	if (EquipState == EEquipState::Unarmed)
		return false;

	bool bAllEmpty = true;
	for (EEquipmentSlotType SlotType : UL1EquipManagerComponent::GetEquipmentSlotsByEquipState(EquipState))
	{
		const FL1EquipmentEntry& Entry = Entries[(int32)SlotType];
		if (Entry.ItemInstance)
		{
			bAllEmpty = false;
			break;
		}
	}
	return bAllEmpty;
}

ALyraCharacter* UL1EquipmentManagerComponent::GetCharacter() const
{
	return Cast<ALyraCharacter>(GetOwner());
}

ALyraPlayerController* UL1EquipmentManagerComponent::GetPlayerController() const
{
	if (ALyraCharacter* LyraCharacter = GetCharacter())
	{
		return LyraCharacter->GetLyraPlayerController();
	}
	return nullptr;
}

UL1EquipManagerComponent* UL1EquipmentManagerComponent::GetEquipManager() const
{
	UL1EquipManagerComponent* EquipManager = nullptr;
	if (ALyraCharacter* Character = GetCharacter())
	{
		EquipManager = Character->FindComponentByClass<UL1EquipManagerComponent>();
	}
	return EquipManager;
}

UL1ItemInstance* UL1EquipmentManagerComponent::GetItemInstance(EEquipmentSlotType EquipmentSlotType) const
{
	if (EquipmentSlotType == EEquipmentSlotType::Count)
		return nullptr;

	const FL1EquipmentEntry& Entry = Entries[(int32)EquipmentSlotType];
	return Entry.GetItemInstance();
}

int32 UL1EquipmentManagerComponent::GetItemCount(EEquipmentSlotType EquipmentSlotType) const
{
	if (EquipmentSlotType == EEquipmentSlotType::Count)
		return 0;

	const FL1EquipmentEntry& Entry = Entries[(int32)EquipmentSlotType];
	return Entry.GetItemCount();
}

const TArray<FL1EquipmentEntry>& UL1EquipmentManagerComponent::GetEntries() const
{
	return Entries;
}

void UL1EquipmentManagerComponent::GetAllWeaponItemInstances(TArray<UL1ItemInstance*>& OutItemInstances) const
{
	OutItemInstances.Reset();

	for (int32 i = 0; i < (int32)EEquipmentSlotType::Count; i++)
	{
		if (IsWeaponSlot((EEquipmentSlotType)i) == false)
			continue;

		const FL1EquipmentEntry& Entry = Entries[i];
		if (Entry.ItemInstance)
		{
			OutItemInstances.Add(Entry.ItemInstance);
		}
	}
}
