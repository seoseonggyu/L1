#include "L1EquipmentManagerComponent.h"

#include "Character/LyraCharacter.h"
#include "Item/Managers/L1EquipManagerComponent.h"
#include "Item/Managers/L1InventoryManagerComponent.h"
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

void UL1EquipmentManagerComponent::BroadcastChangedMessage(EEquipmentSlotType EquipmentSlotType, UL1ItemInstance* ItemInstance, int32 ItemCount)
{
	if (OnEquipmentEntryChanged.IsBound())
	{
		OnEquipmentEntryChanged.Broadcast(EquipmentSlotType, ItemInstance, ItemCount);
	}
}

int32 UL1EquipmentManagerComponent::CanMoveOrMergeEquipment(UL1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType ToEquipmentSlotType) const
{
	if (OtherComponent == nullptr)
		return 0;

	if (FromEquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand || FromEquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand || FromEquipmentSlotType == EEquipmentSlotType::Count)
		return 0;

	const UL1ItemInstance* FromItemInstance = OtherComponent->GetItemInstance(FromEquipmentSlotType);
	const int32 FromItemCount = OtherComponent->GetItemCount(FromEquipmentSlotType);

	if (FromItemInstance == nullptr)
		return 0;

	if (this == OtherComponent && FromEquipmentSlotType == ToEquipmentSlotType)
		return FromItemCount;

	return CanAddEquipment(FromItemInstance->GetItemTemplateID(), FromItemInstance->GetItemRarity(), FromItemCount, ToEquipmentSlotType);
}

int32 UL1EquipmentManagerComponent::CanMoveOrMergeEquipment(UL1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, EEquipmentSlotType ToEquipmentSlotType) const
{
	if (OtherComponent == nullptr)
		return 0;

	const FIntPoint& FromInventorySlotCount = OtherComponent->GetInventorySlotCount();
	if (FromItemSlotPos.X < 0 || FromItemSlotPos.Y < 0 || FromItemSlotPos.X >= FromInventorySlotCount.X || FromItemSlotPos.Y >= FromInventorySlotCount.Y)
		return 0;

	const UL1ItemInstance* FromItemInstance = OtherComponent->GetItemInstance(FromItemSlotPos);
	const int32 FromItemCount = OtherComponent->GetItemCount(FromItemSlotPos);

	if (FromItemInstance == nullptr)
		return 0;

	return CanAddEquipment(FromItemInstance->GetItemTemplateID(), FromItemInstance->GetItemRarity(), FromItemCount, ToEquipmentSlotType);
}

int32 UL1EquipmentManagerComponent::CanMoveOrMergeEquipment_Quick(UL1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType& OutToEquipmentSlotType) const
{
	OutToEquipmentSlotType = EEquipmentSlotType::Count;

	if (OtherComponent == nullptr)
		return 0;

	if (FromEquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand || FromEquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand || FromEquipmentSlotType == EEquipmentSlotType::Count)
		return 0;

	if (this == OtherComponent)
		return 0;

	const UL1ItemInstance* FromItemInstance = OtherComponent->GetItemInstance(FromEquipmentSlotType);
	const int32 FromItemCount = OtherComponent->GetItemCount(FromEquipmentSlotType);

	if (FromItemInstance == nullptr)
		return 0;

	return CanMoveOrMergeEquipment_Quick(FromItemInstance->GetItemTemplateID(), FromItemInstance->GetItemRarity(), FromItemCount, OutToEquipmentSlotType);
}

int32 UL1EquipmentManagerComponent::CanMoveOrMergeEquipment_Quick(UL1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, EEquipmentSlotType& OutToEquipmentSlotType) const
{
	OutToEquipmentSlotType = EEquipmentSlotType::Count;

	if (OtherComponent == nullptr)
		return 0;

	const FIntPoint& FromInventorySlotCount = OtherComponent->GetInventorySlotCount();
	if (FromItemSlotPos.X < 0 || FromItemSlotPos.Y < 0 || FromItemSlotPos.X >= FromInventorySlotCount.X || FromItemSlotPos.Y >= FromInventorySlotCount.Y)
		return 0;

	const UL1ItemInstance* FromItemInstance = OtherComponent->GetItemInstance(FromItemSlotPos);
	const int32 FromItemCount = OtherComponent->GetItemCount(FromItemSlotPos);

	if (FromItemInstance == nullptr)
		return 0;

	return CanMoveOrMergeEquipment_Quick(FromItemInstance->GetItemTemplateID(), FromItemInstance->GetItemRarity(), FromItemCount, OutToEquipmentSlotType);
}

int32 UL1EquipmentManagerComponent::CanMoveOrMergeEquipment_Quick(int32 FromItemTemplateID, EItemRarity FromItemRarity, int32 FromItemCount, EEquipmentSlotType& OutToEquipmentSlotType) const
{
	OutToEquipmentSlotType = EEquipmentSlotType::Count;

	if (FromItemTemplateID <= 0 || FromItemRarity == EItemRarity::Count || FromItemCount <= 0)
		return 0;

	const UL1ItemTemplate& FromItemTemplate = UL1ItemData::Get().FindItemTemplateByID(FromItemTemplateID);
	const UL1ItemFragment_Equipable* FromEquippableFragment = FromItemTemplate.FindFragmentByClass<UL1ItemFragment_Equipable>();
	if (FromEquippableFragment == nullptr)
		return 0;

	if (FromEquippableFragment->EquipmentType == EEquipmentType::Weapon)
	{
		const UL1ItemFragment_Equipable_Weapon* FromWeaponFragment = Cast<UL1ItemFragment_Equipable_Weapon>(FromEquippableFragment);

		for (int32 i = 0; i < (int32)EWeaponSlotType::Count; i++)
		{
			EEquipmentSlotType ToEquipmentSlotType = UL1EquipManagerComponent::ConvertToEquipmentSlotType(FromWeaponFragment->WeaponHandType, (EWeaponSlotType)i);
			int32 MovableCount = CanAddEquipment(FromItemTemplateID, FromItemRarity, FromItemCount, ToEquipmentSlotType);
			if (MovableCount > 0)
			{
				OutToEquipmentSlotType = ToEquipmentSlotType;
				return MovableCount;
			}
		}
	}
	else if (FromEquippableFragment->EquipmentType == EEquipmentType::Armor)
	{
		const UL1ItemFragment_Equipable_Armor* FromArmorFragment = Cast<UL1ItemFragment_Equipable_Armor>(FromEquippableFragment);

		EEquipmentSlotType ToEquipmentSlotType = UL1EquipManagerComponent::ConvertToEquipmentSlotType(FromArmorFragment->ArmorType);
		int32 MovableCount = CanAddEquipment(FromItemTemplateID, FromItemRarity, FromItemCount, ToEquipmentSlotType);
		if (MovableCount > 0)
		{
			OutToEquipmentSlotType = ToEquipmentSlotType;
			return MovableCount;
		}
	}
	else if (FromEquippableFragment->EquipmentType == EEquipmentType::Utility)
	{
		EEquipmentSlotType ToEquipmentSlotPick = EEquipmentSlotType::Count;

		for (int32 i = 0; i < (int32)EUtilitySlotType::Count; i++)
		{
			EEquipmentSlotType ToEquipmentSlotType = UL1EquipManagerComponent::ConvertToEquipmentSlotType((EUtilitySlotType)i);
			UL1ItemInstance* ToItemInstance = GetItemInstance(ToEquipmentSlotType);
			if (ToItemInstance == nullptr)
			{
				ToEquipmentSlotPick = ToEquipmentSlotType;
				break;
			}
			else
			{
				const int32 ToItemCount = GetItemCount(ToEquipmentSlotType);
				const UL1ItemTemplate& ToItemTemplate = UL1ItemData::Get().FindItemTemplateByID(ToItemInstance->GetItemTemplateID());
				if (ToItemTemplate.MaxStackCount > 1 && ToItemCount < ToItemTemplate.MaxStackCount && ToItemInstance->GetItemRarity() == FromItemRarity && ToItemInstance->GetItemTemplateID() == FromItemTemplateID)
				{
					ToEquipmentSlotPick = ToEquipmentSlotType;
					break;
				}
			}
		}

		if (ToEquipmentSlotPick == EEquipmentSlotType::Count)
			return 0;

		int32 MovableCount = CanAddEquipment(FromItemTemplateID, FromItemRarity, FromItemCount, ToEquipmentSlotPick);
		if (MovableCount > 0)
		{
			OutToEquipmentSlotType = ToEquipmentSlotPick;
			return MovableCount;
		}
	}

	return 0;
}

bool UL1EquipmentManagerComponent::CanSwapEquipment(UL1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType ToEquipmentSlotType) const
{
	if (OtherComponent == nullptr || FromEquipmentSlotType == EEquipmentSlotType::Count)
		return false;

	const TArray<FL1EquipmentEntry>& FromEntries = OtherComponent->GetEntries();
	const FL1EquipmentEntry& FromEntry = FromEntries[(int32)FromEquipmentSlotType];
	UL1ItemInstance* FromItemInstance = FromEntry.GetItemInstance();

	if (FromItemInstance == nullptr)
		return false;

	const UL1ItemFragment_Equipable* FromEquippableFragment = FromItemInstance->FindFragmentByClass<UL1ItemFragment_Equipable>();
	if (FromEquippableFragment == nullptr)
		return false;

	if (this == OtherComponent && FromEquipmentSlotType == ToEquipmentSlotType)
		return true;

	const UL1ItemInstance* ToItemInstance = GetItemInstance(ToEquipmentSlotType);
	if (ToItemInstance == nullptr)
		return false;

	if (FromEquippableFragment->EquipmentType == EEquipmentType::Weapon)
	{
		const UL1ItemFragment_Equipable_Weapon* FromWeaponFragment = Cast<UL1ItemFragment_Equipable_Weapon>(FromEquippableFragment);
		if (FromWeaponFragment == nullptr)
			return false;

		EWeaponHandType FromWeaponHandType = FromWeaponFragment->WeaponHandType;
		if (IsSameWeaponHandType(ToEquipmentSlotType, FromWeaponHandType) == false)
			return false;

		return true;
	}
	else if (FromEquippableFragment->EquipmentType == EEquipmentType::Armor)
	{
		if (FromEquipmentSlotType != ToEquipmentSlotType)
			return false;

		return true;
	}
	else if (FromEquippableFragment->EquipmentType == EEquipmentType::Utility)
	{
		if (IsUtilitySlot(ToEquipmentSlotType) == false)
			return false;

		return true;
	}

	return false;
}

bool UL1EquipmentManagerComponent::CanSwapEquipment(UL1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, EEquipmentSlotType ToEquipmentSlotType, FIntPoint& OutToItemSlotPos)
{
	if (OtherComponent == nullptr)
		return false;

	const FIntPoint& FromInventorySlotCount = OtherComponent->GetInventorySlotCount();
	if (FromItemSlotPos.X < 0 || FromItemSlotPos.Y < 0 || FromItemSlotPos.X >= FromInventorySlotCount.X || FromItemSlotPos.Y >= FromInventorySlotCount.Y)
		return false;

	if (ToEquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand || ToEquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand || ToEquipmentSlotType == EEquipmentSlotType::Count)
		return false;

	UL1ItemInstance* FromItemInstance = OtherComponent->GetItemInstance(FromItemSlotPos);
	if (FromItemInstance == nullptr)
		return false;

	const UL1ItemInstance* ToItemInstance = GetItemInstance(ToEquipmentSlotType);
	if (ToItemInstance == nullptr)
		return false;

	const UL1ItemFragment_Equipable* FromEquippableFragment = FromItemInstance->FindFragmentByClass<UL1ItemFragment_Equipable>();
	if (FromEquippableFragment == nullptr)
		return false;

	if (FromEquippableFragment->EquipmentType == EEquipmentType::Weapon)
	{
		const UL1ItemFragment_Equipable_Weapon* FromWeaponFragment = Cast<UL1ItemFragment_Equipable_Weapon>(FromEquippableFragment);
		if (FromWeaponFragment == nullptr)
			return false;

		EWeaponHandType FromWeaponHandType = FromWeaponFragment->WeaponHandType;
		if (IsSameWeaponHandType(ToEquipmentSlotType, FromWeaponHandType) == false)
			return false;
	}
	else if (FromEquippableFragment->EquipmentType == EEquipmentType::Armor)
	{
		const UL1ItemFragment_Equipable_Armor* FromArmorFragment = Cast<UL1ItemFragment_Equipable_Armor>(FromEquippableFragment);
		EEquipmentSlotType FromEquipmentSlotType = UL1EquipManagerComponent::ConvertToEquipmentSlotType(FromArmorFragment->ArmorType);

		if (FromEquipmentSlotType != ToEquipmentSlotType)
			return false;
	}
	else if (FromEquippableFragment->EquipmentType == EEquipmentType::Utility)
	{
		if (IsUtilitySlot(ToEquipmentSlotType) == false)
			return false;
	}

	const UL1ItemTemplate& FromItemTemplate = UL1ItemData::Get().FindItemTemplateByID(FromItemInstance->GetItemTemplateID());
	const FIntPoint& FromSlotCount = FromItemTemplate.SlotCount;

	const UL1ItemTemplate& ToItemTemplate = UL1ItemData::Get().FindItemTemplateByID(ToItemInstance->GetItemTemplateID());
	const FIntPoint& ToSlotCount = ToItemTemplate.SlotCount;

	TArray<bool> TempSlotChecks = OtherComponent->SlotChecks;
	OtherComponent->MarkSlotChecks(TempSlotChecks, false, FromItemSlotPos, FromSlotCount);

	if (OtherComponent->IsEmpty(TempSlotChecks, FromItemSlotPos, ToSlotCount))
	{
		OutToItemSlotPos = FromItemSlotPos;
		return true;
	}
	else
	{
		const FIntPoint StartSlotPos = FIntPoint::ZeroValue;
		const FIntPoint EndSlotPos = FromInventorySlotCount - ToSlotCount;

		for (int32 y = StartSlotPos.Y; y <= EndSlotPos.Y; y++)
		{
			for (int32 x = StartSlotPos.X; x <= EndSlotPos.X; x++)
			{
				int32 Index = y * FromInventorySlotCount.X + x;
				if (TempSlotChecks.IsValidIndex(Index) == false || TempSlotChecks[Index])
					continue;

				FIntPoint ToItemSlotPos = FIntPoint(x, y);
				if (OtherComponent->IsEmpty(TempSlotChecks, ToItemSlotPos, ToSlotCount))
				{
					OutToItemSlotPos = ToItemSlotPos;
					return true;
				}
			}
		}
	}

	return false;
}

bool UL1EquipmentManagerComponent::CanSwapEquipment_Quick(UL1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType& OutToEquipmentSlotType) const
{
	OutToEquipmentSlotType = EEquipmentSlotType::Count;

	if (OtherComponent == nullptr)
		return false;

	if (FromEquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand || FromEquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand || FromEquipmentSlotType == EEquipmentSlotType::Count)
		return false;

	UL1ItemInstance* FromItemInstance = OtherComponent->GetItemInstance(FromEquipmentSlotType);
	if (FromItemInstance == nullptr)
		return false;

	const UL1ItemFragment_Equipable* FromEquippableFragment = FromItemInstance->FindFragmentByClass<UL1ItemFragment_Equipable>();
	if (FromEquippableFragment == nullptr)
		return false;

	if (FromEquippableFragment->EquipmentType == EEquipmentType::Weapon)
	{
		const UL1ItemFragment_Equipable_Weapon* FromWeaponFragment = Cast<UL1ItemFragment_Equipable_Weapon>(FromEquippableFragment);
		if (FromWeaponFragment == nullptr)
			return false;

		if (FindPairItemInstance(FromItemInstance, OutToEquipmentSlotType))
			return true;
	}
	else if (FromEquippableFragment->EquipmentType == EEquipmentType::Armor)
	{
		const UL1ItemFragment_Equipable_Armor* FromArmorFragment = Cast<UL1ItemFragment_Equipable_Armor>(FromEquippableFragment);
		EEquipmentSlotType ToEquipmentSlotType = UL1EquipManagerComponent::ConvertToEquipmentSlotType(FromArmorFragment->ArmorType);

		if (GetItemInstance(ToEquipmentSlotType))
		{
			OutToEquipmentSlotType = ToEquipmentSlotType;
			return true;
		}
	}

	return false;
}

bool UL1EquipmentManagerComponent::CanSwapEquipment_Quick(UL1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, EEquipmentSlotType& OutToEquipmentSlotType, FIntPoint& OutToItemSlotPos)
{
	OutToEquipmentSlotType = EEquipmentSlotType::Count;

	if (OtherComponent == nullptr)
		return false;

	const FIntPoint& FromInventorySlotCount = OtherComponent->GetInventorySlotCount();
	if (FromItemSlotPos.X < 0 || FromItemSlotPos.Y < 0 || FromItemSlotPos.X >= FromInventorySlotCount.X || FromItemSlotPos.Y >= FromInventorySlotCount.Y)
		return false;

	UL1ItemInstance* FromItemInstance = OtherComponent->GetItemInstance(FromItemSlotPos);
	if (FromItemInstance == nullptr)
		return false;

	const UL1ItemFragment_Equipable* FromEquippableFragment = FromItemInstance->FindFragmentByClass<UL1ItemFragment_Equipable>();
	if (FromEquippableFragment == nullptr)
		return false;

	const UL1ItemInstance* ToItemInstance = nullptr;

	if (FromEquippableFragment->EquipmentType == EEquipmentType::Weapon)
	{
		const UL1ItemFragment_Equipable_Weapon* FromWeaponFragment = Cast<UL1ItemFragment_Equipable_Weapon>(FromEquippableFragment);
		if (FromWeaponFragment == nullptr)
			return false;

		ToItemInstance = FindPairItemInstance(FromItemInstance, OutToEquipmentSlotType);
	}
	else if (FromEquippableFragment->EquipmentType == EEquipmentType::Armor)
	{
		const UL1ItemFragment_Equipable_Armor* FromArmorFragment = Cast<UL1ItemFragment_Equipable_Armor>(FromEquippableFragment);
		OutToEquipmentSlotType = UL1EquipManagerComponent::ConvertToEquipmentSlotType(FromArmorFragment->ArmorType);

		ToItemInstance = GetItemInstance(OutToEquipmentSlotType);
	}

	if (ToItemInstance == nullptr)
		return false;

	const UL1ItemTemplate& FromItemTemplate = UL1ItemData::Get().FindItemTemplateByID(FromItemInstance->GetItemTemplateID());
	const FIntPoint& FromSlotCount = FromItemTemplate.SlotCount;

	const UL1ItemTemplate& ToItemTemplate = UL1ItemData::Get().FindItemTemplateByID(ToItemInstance->GetItemTemplateID());
	const FIntPoint& ToSlotCount = ToItemTemplate.SlotCount;

	TArray<bool> TempSlotChecks = OtherComponent->SlotChecks;
	OtherComponent->MarkSlotChecks(TempSlotChecks, false, FromItemSlotPos, FromSlotCount);

	if (OtherComponent->IsEmpty(TempSlotChecks, FromItemSlotPos, ToSlotCount))
	{
		OutToItemSlotPos = FromItemSlotPos;
		return true;
	}
	else
	{
		const FIntPoint StartSlotPos = FIntPoint::ZeroValue;
		const FIntPoint EndSlotPos = FromInventorySlotCount - ToSlotCount;

		for (int32 y = StartSlotPos.Y; y <= EndSlotPos.Y; y++)
		{
			for (int32 x = StartSlotPos.X; x <= EndSlotPos.X; x++)
			{
				int32 Index = y * FromInventorySlotCount.X + x;
				if (TempSlotChecks.IsValidIndex(Index) == false || TempSlotChecks[Index])
					continue;

				FIntPoint ToItemSlotPos = FIntPoint(x, y);
				if (OtherComponent->IsEmpty(TempSlotChecks, ToItemSlotPos, ToSlotCount))
				{
					OutToItemSlotPos = ToItemSlotPos;
					return true;
				}
			}
		}
	}

	return false;
}

int32 UL1EquipmentManagerComponent::CanAddEquipment(int32 ItemTemplateID, EItemRarity ItemRarity, int32 ItemCount, EEquipmentSlotType ToEquipmentSlotType) const
{
	if (ItemTemplateID <= 0 || ItemRarity == EItemRarity::Count || ItemCount <= 0)
		return 0;

	if (ToEquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand || ToEquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand || ToEquipmentSlotType == EEquipmentSlotType::Count)
		return 0;

	const UL1ItemTemplate& ItemTemplate = UL1ItemData::Get().FindItemTemplateByID(ItemTemplateID);
	const UL1ItemFragment_Equipable* FromEquippableFragment = ItemTemplate.FindFragmentByClass<UL1ItemFragment_Equipable>();
	if (FromEquippableFragment == nullptr)
		return 0;

	const UL1ItemInstance* ToItemInstance = GetItemInstance(ToEquipmentSlotType);
	const int32 ToItemCount = GetItemCount(ToEquipmentSlotType);

	if (ToItemInstance)
	{
		const int32 ToTemplateID = ToItemInstance->GetItemTemplateID();
		if (ItemTemplateID != ToTemplateID)
			return 0;

		if (ItemRarity != ToItemInstance->GetItemRarity())
			return 0;

		if (ItemTemplate.MaxStackCount < 2)
			return 0;

		return FMath::Min(ItemCount + ToItemCount, ItemTemplate.MaxStackCount) - ToItemCount;
	}
	else
	{
		if (FromEquippableFragment->EquipmentType == EEquipmentType::Weapon)
		{
			const UL1ItemFragment_Equipable_Weapon* FromWeaponFragment = Cast<UL1ItemFragment_Equipable_Weapon>(FromEquippableFragment);
			EWeaponHandType FromWeaponHandType = FromWeaponFragment->WeaponHandType;

			if (IsSameWeaponHandType(ToEquipmentSlotType, FromWeaponHandType) == false)
				return 0;

			if (IsPrimaryWeaponSlot(ToEquipmentSlotType))
			{
				if (FromWeaponHandType == EWeaponHandType::LeftHand || FromWeaponHandType == EWeaponHandType::RightHand)
				{
					return (GetItemInstance(EEquipmentSlotType::Primary_TwoHand) == nullptr) ? ItemCount : 0;
				}
				else if (FromWeaponHandType == EWeaponHandType::TwoHand)
				{
					return (GetItemInstance(EEquipmentSlotType::Primary_LeftHand) == nullptr && GetItemInstance(EEquipmentSlotType::Primary_RightHand) == nullptr) ? ItemCount : 0;
				}
			}
			else if (IsSecondaryWeaponSlot(ToEquipmentSlotType))
			{
				if (FromWeaponHandType == EWeaponHandType::LeftHand || FromWeaponHandType == EWeaponHandType::RightHand)
				{
					return (GetItemInstance(EEquipmentSlotType::Secondary_TwoHand) == nullptr) ? ItemCount : 0;
				}
				else if (FromWeaponHandType == EWeaponHandType::TwoHand)
				{
					return (GetItemInstance(EEquipmentSlotType::Secondary_LeftHand) == nullptr && GetItemInstance(EEquipmentSlotType::Secondary_RightHand) == nullptr) ? ItemCount : 0;
				}
			}
		}
		else if (FromEquippableFragment->EquipmentType == EEquipmentType::Armor)
		{
			const UL1ItemFragment_Equipable_Armor* FromArmorFragment = Cast<UL1ItemFragment_Equipable_Armor>(FromEquippableFragment);
			return IsSameArmorType(ToEquipmentSlotType, FromArmorFragment->ArmorType) ? ItemCount : 0;
		}
		else if (FromEquippableFragment->EquipmentType == EEquipmentType::Utility)
		{
			return IsUtilitySlot(ToEquipmentSlotType) ? ItemCount : 0;
		}
	}

	return 0;
}

void UL1EquipmentManagerComponent::AddUnarmedEquipments(TSubclassOf<UL1ItemTemplate> LeftHandClass, TSubclassOf<UL1ItemTemplate> RightHandClass)
{
	SetEquipment(EEquipmentSlotType::Unarmed_LeftHand, LeftHandClass, EItemRarity::Poor, 1);
	SetEquipment(EEquipmentSlotType::Unarmed_RightHand, RightHandClass, EItemRarity::Poor, 1);
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

	BroadcastChangedMessage(EquipmentSlotType, ItemInstance, ItemCount);
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

	BroadcastChangedMessage(EquipmentSlotType, Entry.GetItemInstance(), ItemCount);
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

	BroadcastChangedMessage(EquipmentSlotType, AddedItemInstance, ItemCount);

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
