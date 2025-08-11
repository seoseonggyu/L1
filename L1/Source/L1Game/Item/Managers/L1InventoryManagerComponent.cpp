#include "L1InventoryManagerComponent.h"

#include "Item/L1ItemInstance.h"
#include "Item/L1ItemTemplate.h"
#include "Item/Managers/L1EquipmentManagerComponent.h"
#include "Data/L1ItemData.h"
#include "Network/L1NetworkCharacter.h"

UL1ItemInstance* FL1InventoryEntry::Init(int32 InItemTemplateID, int32 InItemCount, EItemRarity InItemRarity)
{
	check(InItemTemplateID > 0 && InItemCount > 0 && InItemRarity != EItemRarity::Count);

	UL1ItemInstance* NewItemInstance = NewObject<UL1ItemInstance>();
	NewItemInstance->Init(InItemTemplateID, InItemRarity);
	Init(NewItemInstance, InItemCount);

	return NewItemInstance;
}

void FL1InventoryEntry::Init(UL1ItemInstance* InItemInstance, int32 InItemCount)
{
	check(InItemInstance && InItemCount > 0);

	ItemInstance = InItemInstance;

	const UL1ItemTemplate& ItemTemplate = UL1ItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
	ItemCount = FMath::Clamp(InItemCount, 1, ItemTemplate.MaxStackCount);
}

UL1ItemInstance* FL1InventoryEntry::Reset()
{
	UL1ItemInstance* RemovedItemInstance = ItemInstance;
	ItemInstance = nullptr;
	ItemCount = 0;

	return RemovedItemInstance;
}

UL1InventoryManagerComponent::UL1InventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
}

void UL1InventoryManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (GetOwner())
	{
		Entries.SetNum(InventorySlotCount.X * InventorySlotCount.Y);
		SlotChecks.SetNumZeroed(InventorySlotCount.X * InventorySlotCount.Y);
	}
}

void UL1InventoryManagerComponent::BroadcastChangedMessage(const FIntPoint& ItemSlotPos, UL1ItemInstance* ItemInstance, int32 ItemCount)
{
	if (OnInventoryEntryChanged.IsBound())
	{
		OnInventoryEntryChanged.Broadcast(ItemSlotPos, ItemInstance, ItemCount);
	}
}

int32 UL1InventoryManagerComponent::CanMoveOrMergeItem(UL1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, const FIntPoint& ToItemSlotPos) const
{
	if (OtherComponent == nullptr)
		return 0;

	const FIntPoint& FromInventorySlotCount = OtherComponent->GetInventorySlotCount();
	if (FromItemSlotPos.X < 0 || FromItemSlotPos.Y < 0 || FromItemSlotPos.X >= FromInventorySlotCount.X || FromItemSlotPos.Y >= FromInventorySlotCount.Y)
		return 0;

	if (ToItemSlotPos.X < 0 || ToItemSlotPos.Y < 0 || ToItemSlotPos.X >= InventorySlotCount.X || ToItemSlotPos.Y >= InventorySlotCount.Y)
		return 0;

	const UL1ItemInstance* FromItemInstance = OtherComponent->GetItemInstance(FromItemSlotPos);
	const int32 FromItemCount = OtherComponent->GetItemCount(FromItemSlotPos);

	if (this == OtherComponent && FromItemSlotPos == ToItemSlotPos)
		return FromItemCount;

	if (FromItemInstance == nullptr || FromItemCount <= 0)
		return 0;

	const UL1ItemInstance* ToItemInstance = GetItemInstance(ToItemSlotPos);
	const int32 ToItemCount = GetItemCount(ToItemSlotPos);

	const int32 FromTemplateID = FromItemInstance->GetItemTemplateID();
	const UL1ItemTemplate& FromItemTemplate = UL1ItemData::Get().FindItemTemplateByID(FromTemplateID);

	if (ToItemInstance)
	{
		const int32 ToTemplateID = ToItemInstance->GetItemTemplateID();
		if (FromTemplateID != ToTemplateID)
			return 0;

		if (FromItemInstance->GetItemRarity() != ToItemInstance->GetItemRarity())
			return 0;

		if (FromItemTemplate.MaxStackCount < 2)
			return 0;

		return FMath::Min(FromItemCount + ToItemCount, FromItemTemplate.MaxStackCount) - ToItemCount;
	}
	else
	{
		const FIntPoint& FromItemSlotCount = FromItemTemplate.SlotCount;
		if (ToItemSlotPos.X + FromItemSlotCount.X > InventorySlotCount.X || ToItemSlotPos.Y + FromItemSlotCount.Y > InventorySlotCount.Y)
			return 0;

		if (this == OtherComponent)
		{
			TArray<bool> TempSlotChecks = SlotChecks;
			MarkSlotChecks(TempSlotChecks, false, FromItemSlotPos, FromItemSlotCount);

			return IsEmpty(TempSlotChecks, ToItemSlotPos, FromItemSlotCount) ? FromItemCount : 0;
		}
		else
		{
			return IsEmpty(ToItemSlotPos, FromItemSlotCount) ? FromItemCount : 0;
		}
	}
}

int32 UL1InventoryManagerComponent::CanMoveOrMergeItem(UL1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, const FIntPoint& ToItemSlotPos) const
{
	if (OtherComponent == nullptr)
		return 0;

	if (FromEquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand || FromEquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand || FromEquipmentSlotType == EEquipmentSlotType::Count)
		return 0;

	if (ToItemSlotPos.X < 0 || ToItemSlotPos.Y < 0 || ToItemSlotPos.X >= InventorySlotCount.X || ToItemSlotPos.Y >= InventorySlotCount.Y)
		return 0;

	const UL1ItemInstance* FromItemInstance = OtherComponent->GetItemInstance(FromEquipmentSlotType);
	const int32 FromItemCount = OtherComponent->GetItemCount(FromEquipmentSlotType);

	if (FromItemInstance == nullptr || FromItemCount <= 0)
		return 0;

	const UL1ItemInstance* ToItemInstance = GetItemInstance(ToItemSlotPos);
	const int32 ToItemCount = GetItemCount(ToItemSlotPos);

	const int32 FromTemplateID = FromItemInstance->GetItemTemplateID();
	const UL1ItemTemplate& FromItemTemplate = UL1ItemData::Get().FindItemTemplateByID(FromTemplateID);

	if (ToItemInstance)
	{
		const int32 ToTemplateID = ToItemInstance->GetItemTemplateID();
		if (FromTemplateID != ToTemplateID)
			return 0;

		if (FromItemInstance->GetItemRarity() != ToItemInstance->GetItemRarity())
			return 0;

		if (FromItemTemplate.MaxStackCount < 2)
			return 0;

		return FMath::Min(FromItemCount + ToItemCount, FromItemTemplate.MaxStackCount) - ToItemCount;
	}
	else
	{
		const FIntPoint& FromItemSlotCount = FromItemTemplate.SlotCount;
		if (ToItemSlotPos.X + FromItemSlotCount.X > InventorySlotCount.X || ToItemSlotPos.Y + FromItemSlotCount.Y > InventorySlotCount.Y)
			return 0;

		return IsEmpty(ToItemSlotPos, FromItemSlotCount) ? FromItemCount : 0;
	}
}

int32 UL1InventoryManagerComponent::CanMoveOrMergeItem_Quick(UL1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, TArray<FIntPoint>& OutToItemSlotPoses, TArray<int32>& OutToItemCounts) const
{
	OutToItemSlotPoses.Reset();
	OutToItemCounts.Reset();

	if (OtherComponent == nullptr || this == OtherComponent)
		return 0;

	const FIntPoint& FromInventorySlotCount = OtherComponent->GetInventorySlotCount();
	if (FromItemSlotPos.X < 0 || FromItemSlotPos.Y < 0 || FromItemSlotPos.X >= FromInventorySlotCount.X || FromItemSlotPos.Y >= FromInventorySlotCount.Y)
		return 0;

	const UL1ItemInstance* FromItemInstance = OtherComponent->GetItemInstance(FromItemSlotPos);
	const int32 FromItemCount = OtherComponent->GetItemCount(FromItemSlotPos);

	if (FromItemInstance == nullptr)
		return 0;

	return CanAddItem(FromItemInstance->GetItemTemplateID(), FromItemInstance->GetItemRarity(), FromItemCount, OutToItemSlotPoses, OutToItemCounts);
}

int32 UL1InventoryManagerComponent::CanMoveOrMergeItem_Quick(UL1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, TArray<FIntPoint>& OutToItemSlotPoses, TArray<int32>& OutToItemCounts) const
{
	OutToItemSlotPoses.Reset();
	OutToItemCounts.Reset();

	if (OtherComponent == nullptr)
		return 0;

	if (FromEquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand || FromEquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand || FromEquipmentSlotType == EEquipmentSlotType::Count)
		return 0;

	const UL1ItemInstance* FromItemInstance = OtherComponent->GetItemInstance(FromEquipmentSlotType);
	const int32 FromItemCount = OtherComponent->GetItemCount(FromEquipmentSlotType);

	if (FromItemInstance == nullptr)
		return 0;

	return CanAddItem(FromItemInstance->GetItemTemplateID(), FromItemInstance->GetItemRarity(), FromItemCount, OutToItemSlotPoses, OutToItemCounts);
}

int32 UL1InventoryManagerComponent::CanAddItem(int32 ItemTemplateID, EItemRarity ItemRarity, int32 ItemCount, TArray<FIntPoint>& OutToItemSlotPoses, TArray<int32>& OutToItemCounts) const
{
	OutToItemSlotPoses.Reset();
	OutToItemCounts.Reset();

	if (ItemTemplateID <= 0 || ItemRarity == EItemRarity::Count || ItemCount <= 0)
		return 0;

	const UL1ItemTemplate& ItemTemplate = UL1ItemData::Get().FindItemTemplateByID(ItemTemplateID);
	int32 LeftItemCount = ItemCount;

	if (ItemTemplate.MaxStackCount > 1)
	{
		for (int32 i = 0; i < Entries.Num(); i++)
		{
			const FL1InventoryEntry& ToEntry = Entries[i];
			const UL1ItemInstance* ToItemInstance = ToEntry.GetItemInstance();
			const int32 ToItemCount = ToEntry.GetItemCount();

			if (ToItemInstance == nullptr)
				continue;

			if (ToItemInstance->GetItemTemplateID() != ItemTemplateID)
				continue;

			if (ToItemInstance->GetItemRarity() != ItemRarity)
				continue;

			if (int32 AddCount = FMath::Min(ToItemCount + LeftItemCount, ItemTemplate.MaxStackCount) - ToItemCount)
			{
				OutToItemSlotPoses.Emplace(i % InventorySlotCount.X, i / InventorySlotCount.X);
				OutToItemCounts.Emplace(AddCount);
				LeftItemCount -= AddCount;

				if (LeftItemCount == 0)
					return ItemCount;
			}
		}
	}

	const FIntPoint& ItemSlotCount = ItemTemplate.SlotCount;
	TArray<bool> TempSlotChecks = SlotChecks;

	const FIntPoint StartSlotPos = FIntPoint::ZeroValue;
	const FIntPoint EndSlotPos = InventorySlotCount - ItemSlotCount;

	for (int32 y = StartSlotPos.Y; y <= EndSlotPos.Y; y++)
	{
		for (int32 x = StartSlotPos.X; x <= EndSlotPos.X; x++)
		{
			int32 Index = y * InventorySlotCount.X + x;
			if (TempSlotChecks.IsValidIndex(Index) == false || TempSlotChecks[Index])
				continue;

			FIntPoint ItemSlotPos = FIntPoint(x, y);
			if (IsEmpty(TempSlotChecks, ItemSlotPos, ItemSlotCount))
			{
				MarkSlotChecks(TempSlotChecks, true, ItemSlotPos, ItemSlotCount);

				int32 AddCount = FMath::Min(LeftItemCount, ItemTemplate.MaxStackCount);
				OutToItemSlotPoses.Emplace(ItemSlotPos);
				OutToItemCounts.Emplace(AddCount);

				LeftItemCount -= AddCount;

				if (LeftItemCount == 0)
					return ItemCount;
			}
		}
	}

	return ItemCount - LeftItemCount;
}

bool UL1InventoryManagerComponent::CanRemoveItem(int32 ItemTemplateID, int32 ItemCount, TArray<FIntPoint>& OutToItemSlotPoses, TArray<int32>& OutToItemCounts) const
{
	OutToItemSlotPoses.Reset();
	OutToItemCounts.Reset();

	if (ItemTemplateID <= 0 || ItemCount <= 0)
		return false;

	TArray<FIntPoint> ToItemSlotPoses;
	TArray<int32> ToItemCounts;


	for (int32 i = Entries.Num() - 1; i >= 0; i--)
	{
		const FL1InventoryEntry& ToEntry = Entries[i];
		if (ToEntry.ItemInstance == nullptr)
			continue;

		if (ToEntry.ItemInstance->GetItemTemplateID() != ItemTemplateID)
			continue;

		if (ToEntry.ItemCount < ItemCount)
		{
			ToItemSlotPoses.Emplace(i % InventorySlotCount.X, i / InventorySlotCount.X);
			ToItemCounts.Emplace(ToEntry.ItemCount);

			ItemCount -= ToEntry.ItemCount;
		}
		else
		{
			ToItemSlotPoses.Emplace(i % InventorySlotCount.X, i / InventorySlotCount.X);
			ToItemCounts.Emplace(ItemCount);

			OutToItemSlotPoses = ToItemSlotPoses;
			OutToItemCounts = ToItemCounts;
			return true;
		}
	}

	return false;
}

int32 UL1InventoryManagerComponent::TryAddItemByRarity(TSubclassOf<UL1ItemTemplate> ItemTemplateClass, EItemRarity ItemRarity, int32 ItemCount)
{
	check(GetOwner()->HasAuthority());

	if (ItemTemplateClass == nullptr || ItemRarity == EItemRarity::Count || ItemCount <= 0)
		return 0;

	int32 ItemTemplateID = UL1ItemData::Get().FindItemTemplateIDByClass(ItemTemplateClass);
	const UL1ItemTemplate& ItemTemplate = UL1ItemData::Get().FindItemTemplateByID(ItemTemplateID);

	TArray<FIntPoint> ToItemSlotPoses;
	TArray<int32> ToItemCounts;

	int32 AddableItemCount = CanAddItem(ItemTemplateID, ItemRarity, ItemCount, ToItemSlotPoses, ToItemCounts);
	if (AddableItemCount > 0)
	{
		TArray<UL1ItemInstance*> AddedItemInstances;

		for (int32 i = 0; i < ToItemSlotPoses.Num(); i++)
		{
			const FIntPoint& ToItemSlotPos = ToItemSlotPoses[i];
			const int32 ToItemCount = ToItemCounts[i];

			const int32 ToIndex = ToItemSlotPos.Y * InventorySlotCount.X + ToItemSlotPos.X;
			FL1InventoryEntry& ToEntry = Entries[ToIndex];

			if (ToEntry.ItemInstance)
			{
				ToEntry.ItemCount += ToItemCount;
			}
			else
			{
				AddedItemInstances.Add(ToEntry.Init(ItemTemplateID, ToItemCount, ItemRarity));
				MarkSlotChecks(true, ToItemSlotPos, ItemTemplate.SlotCount);
			}
		}
		return AddableItemCount;
	}

	return 0;
}

int32 UL1InventoryManagerComponent::TryAddItemByProbability(TSubclassOf<UL1ItemTemplate> ItemTemplateClass, int32 ItemCount, const TArray<FL1ItemRarityProbability>& ItemProbabilities)
{
	return TryAddItemByRarity(ItemTemplateClass, UL1ItemInstance::DetermineItemRarity(ItemProbabilities), ItemCount);
}

bool UL1InventoryManagerComponent::TryRemoveItem(int32 ItemTemplateID, int32 ItemCount)
{
	if (ItemTemplateID <= 0 || ItemCount <= 0)
		return false;

	const UL1ItemTemplate& ItemTemplate = UL1ItemData::Get().FindItemTemplateByID(ItemTemplateID);

	TArray<FIntPoint> ToItemSlotPoses;
	TArray<int32> ToItemCounts;

	if (CanRemoveItem(ItemTemplateID, ItemCount, ToItemSlotPoses, ToItemCounts))
	{
		TArray<UL1ItemInstance*> RemovedItemInstances;

		for (int32 i = 0; i < ToItemSlotPoses.Num(); i++)
		{
			const FIntPoint& ToItemSlotPos = ToItemSlotPoses[i];
			const int32 ToItemCount = ToItemCounts[i];

			const int32 ToIndex = ToItemSlotPos.Y * InventorySlotCount.X + ToItemSlotPos.X;
			FL1InventoryEntry& ToEntry = Entries[ToIndex];

			ToEntry.ItemCount -= ToItemCount;

			if (ToEntry.ItemCount <= 0)
			{
				MarkSlotChecks(false, ToItemSlotPos, ItemTemplate.SlotCount);
				RemovedItemInstances.Add(ToEntry.Reset());
			}

		}
		return true;
	}

	return false;
}

void UL1InventoryManagerComponent::AddItem_Unsafe(const FIntPoint& ItemSlotPos, UL1ItemInstance* ItemInstance, int32 ItemCount)
{
	const int32 Index = ItemSlotPos.Y * InventorySlotCount.X + ItemSlotPos.X;
	FL1InventoryEntry& Entry = Entries[Index];

	if (Entry.GetItemInstance())
	{
		Entry.ItemCount += ItemCount;
	}
	else
	{
		if (ItemInstance == nullptr)
			return;

		const UL1ItemTemplate& ItemTemplate = UL1ItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
		Entry.Init(ItemInstance, ItemCount);
		MarkSlotChecks(true, ItemSlotPos, ItemTemplate.SlotCount);
	}

	BroadcastChangedMessage(ItemSlotPos, ItemInstance, ItemCount);
}

UL1ItemInstance* UL1InventoryManagerComponent::RemoveItem_Unsafe(const FIntPoint& ItemSlotPos, int32 ItemCount)
{

	const int32 Index = ItemSlotPos.Y * InventorySlotCount.X + ItemSlotPos.X;
	FL1InventoryEntry& Entry = Entries[Index];
	UL1ItemInstance* ItemInstance = Entry.GetItemInstance();

	Entry.ItemCount -= ItemCount;
	if (Entry.GetItemCount() <= 0)
	{
		const UL1ItemTemplate& ItemTemplate = UL1ItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
		MarkSlotChecks(false, ItemSlotPos, ItemTemplate.SlotCount);

		UL1ItemInstance* RemovedItemInstance = Entry.Reset();
	}

	BroadcastChangedMessage(ItemSlotPos, Entry.GetItemInstance(), ItemCount);
	return ItemInstance;
}

void UL1InventoryManagerComponent::MarkSlotChecks(TArray<bool>& InSlotChecks, bool bIsUsing, const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount) const
{
	if (ItemSlotPos.X < 0 || ItemSlotPos.Y < 0)
		return;

	if (ItemSlotPos.X + ItemSlotCount.X > InventorySlotCount.X || ItemSlotPos.Y + ItemSlotCount.Y > InventorySlotCount.Y)
		return;

	const FIntPoint StartSlotPos = ItemSlotPos;
	const FIntPoint EndSlotPos = ItemSlotPos + ItemSlotCount;

	for (int32 y = StartSlotPos.Y; y < EndSlotPos.Y; y++)
	{
		for (int32 x = StartSlotPos.X; x < EndSlotPos.X; x++)
		{
			int32 Index = y * InventorySlotCount.X + x;
			if (InSlotChecks.IsValidIndex(Index))
			{
				InSlotChecks[Index] = bIsUsing;
			}
		}
	}
}

void UL1InventoryManagerComponent::MarkSlotChecks(bool bIsUsing, const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount)
{
	MarkSlotChecks(SlotChecks, bIsUsing, ItemSlotPos, ItemSlotCount);
}

bool UL1InventoryManagerComponent::IsEmpty(const TArray<bool>& InSlotChecks, const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount) const
{
	if (ItemSlotPos.X < 0 || ItemSlotPos.Y < 0)
		return false;

	if (ItemSlotPos.X + ItemSlotCount.X > InventorySlotCount.X || ItemSlotPos.Y + ItemSlotCount.Y > InventorySlotCount.Y)
		return false;

	const FIntPoint StartSlotPos = ItemSlotPos;
	const FIntPoint EndSlotPos = ItemSlotPos + ItemSlotCount;

	for (int32 y = StartSlotPos.Y; y < EndSlotPos.Y; y++)
	{
		for (int32 x = StartSlotPos.X; x < EndSlotPos.X; x++)
		{
			int32 Index = y * InventorySlotCount.X + x;
			if (InSlotChecks.IsValidIndex(Index) == false || InSlotChecks[Index])
				return false;
		}
	}
	return true;
}

bool UL1InventoryManagerComponent::IsEmpty(const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount) const
{
	return IsEmpty(SlotChecks, ItemSlotPos, ItemSlotCount);
}

bool UL1InventoryManagerComponent::IsAllEmpty()
{
	for (FL1InventoryEntry& Entry : Entries)
	{
		if (Entry.GetItemInstance())
			return false;
	}
	return true;
}

UL1ItemInstance* UL1InventoryManagerComponent::GetItemInstance(const FIntPoint& ItemSlotPos) const
{
	if (ItemSlotPos.X < 0 || ItemSlotPos.Y < 0 || ItemSlotPos.X >= InventorySlotCount.X || ItemSlotPos.Y >= InventorySlotCount.Y)
		return nullptr;

	const int32 EntryIndex = ItemSlotPos.Y * InventorySlotCount.X + ItemSlotPos.X;
	const FL1InventoryEntry& Entry = Entries[EntryIndex];

	return Entry.GetItemInstance();
}

int32 UL1InventoryManagerComponent::GetItemCount(const FIntPoint& ItemSlotPos) const
{
	if (ItemSlotPos.X < 0 || ItemSlotPos.Y < 0 || ItemSlotPos.X >= InventorySlotCount.X || ItemSlotPos.Y >= InventorySlotCount.Y)
		return 0;

	const int32 EntryIndex = ItemSlotPos.Y * InventorySlotCount.X + ItemSlotPos.X;
	const FL1InventoryEntry& Entry = Entries[EntryIndex];

	return Entry.GetItemCount();
}

const TArray<FL1InventoryEntry>& UL1InventoryManagerComponent::GetEntries() const
{
	return Entries;
}

int32 UL1InventoryManagerComponent::GetTotalCountByID(int32 ItemTemplateID) const
{
	int32 TotalCount = 0;

	for (const FL1InventoryEntry& Entry : Entries)
	{
		if (UL1ItemInstance* ItemInstance = Entry.ItemInstance)
		{
			if (ItemInstance->GetItemTemplateID() == ItemTemplateID)
			{
				TotalCount += Entry.ItemCount;
			}
		}
	}

	return TotalCount;
}
