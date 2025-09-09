
#include "Network/L1ItemTransferHelper.h"
#include "Item/L1ItemInstance.h"
#include "Item/Fragments/L1ItemFragment_Equipable.h"
#include "Item/Managers/L1EquipmentManagerComponent.h"
#include "Item/Managers/L1InventoryManagerComponent.h"
#include "Character/LyraCharacter.h"

void UL1ItemTransferHelper::EquipmentToInventory(ALyraCharacter* FromPlayer, EEquipmentSlotType FromEquipmentSlotType, ALyraCharacter* ToPlayer, const FIntPoint& ToItemSlotPos, int32 MovableCount)
{
	if (UL1EquipmentManagerComponent* FromEquipmentManager = FromPlayer->GetComponentByClass<UL1EquipmentManagerComponent>())
	{
		UL1ItemInstance* RemovedItemInstance = FromEquipmentManager->RemoveEquipment_Unsafe(FromEquipmentSlotType, MovableCount);
		if (UL1InventoryManagerComponent* ToInventoryManager = ToPlayer->GetComponentByClass<UL1InventoryManagerComponent>())
		{
			ToInventoryManager->AddItem_Unsafe(ToItemSlotPos, RemovedItemInstance, MovableCount);
		}
	}
}

void UL1ItemTransferHelper::InventoryToInventory(ALyraCharacter* FromPlayer, const FIntPoint& FromItemSlotPos, ALyraCharacter* ToPlayer, const FIntPoint& ToItemSlotPos, int32 MovableCount)
{
	if (UL1InventoryManagerComponent* FromInventoryManager = FromPlayer->GetComponentByClass<UL1InventoryManagerComponent>())
	{
		UL1ItemInstance* RemovedItemInstance = FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, MovableCount);
		if (UL1InventoryManagerComponent* ToInventoryManager = ToPlayer->GetComponentByClass<UL1InventoryManagerComponent>())
		{
			ToInventoryManager->AddItem_Unsafe(ToItemSlotPos, RemovedItemInstance, MovableCount);
		}
	}
}

void UL1ItemTransferHelper::InventoryToEquipment(ALyraCharacter* FromPlayer, ALyraCharacter* ToPlayer, EEquipmentSlotType ToEquipmentSlotType, const FIntPoint& FromItemSlotPos, const FIntPoint& ToItemSlotPos, int32 MovableCount)
{
	UL1InventoryManagerComponent* FromInventoryManager = FromPlayer->GetComponentByClass<UL1InventoryManagerComponent>();
	UL1EquipmentManagerComponent* ToEquipmentManager = ToPlayer->GetComponentByClass<UL1EquipmentManagerComponent>();
	if (FromInventoryManager == nullptr || ToEquipmentManager == nullptr) return;

	if (MovableCount)
	{
		UL1ItemInstance* RemovedItemInstance = FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, MovableCount);
		ToEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstance, MovableCount);
	}
	else
	{
		const int32 FromItemCount = FromInventoryManager->GetItemCount(FromItemSlotPos);
		const int32 ToItemCount = ToEquipmentManager->GetItemCount(ToEquipmentSlotType);

		UL1ItemInstance* RemovedItemInstanceFrom = FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, FromItemCount);
		UL1ItemInstance* RemovedItemInstanceTo = ToEquipmentManager->RemoveEquipment_Unsafe(ToEquipmentSlotType, ToItemCount);
		FromInventoryManager->AddItem_Unsafe(ToItemSlotPos, RemovedItemInstanceTo, ToItemCount);
		ToEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstanceFrom, FromItemCount);
	}
}

void UL1ItemTransferHelper::EquipmentToEquipment(ALyraCharacter* FromPlayer, ALyraCharacter* ToPlayer, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType ToEquipmentSlotType, int32 MovableCount)
{
	UL1EquipmentManagerComponent* FromEquipmentManager = FromPlayer->GetComponentByClass<UL1EquipmentManagerComponent>();
	UL1EquipmentManagerComponent* ToEquipmentManager = ToPlayer->GetComponentByClass<UL1EquipmentManagerComponent>();

	if (FromEquipmentManager == nullptr || ToEquipmentManager == nullptr)
		return;
	if (FromEquipmentManager == ToEquipmentManager && FromEquipmentSlotType == ToEquipmentSlotType)
		return;

	if (MovableCount > 0)
	{
		UL1ItemInstance* RemovedItemInstance = FromEquipmentManager->RemoveEquipment_Unsafe(FromEquipmentSlotType, MovableCount);
		ToEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstance, MovableCount);
	}
	else
	{
		const int32 FromItemCount = FromEquipmentManager->GetItemCount(FromEquipmentSlotType);
		const int32 ToItemCount = ToEquipmentManager->GetItemCount(ToEquipmentSlotType);

		UL1ItemInstance* RemovedItemInstanceFrom = FromEquipmentManager->RemoveEquipment_Unsafe(FromEquipmentSlotType, FromItemCount);
		UL1ItemInstance* RemovedItemInstanceTo = ToEquipmentManager->RemoveEquipment_Unsafe(ToEquipmentSlotType, ToItemCount);
		FromEquipmentManager->AddEquipment_Unsafe(FromEquipmentSlotType, RemovedItemInstanceTo, ToItemCount);
		ToEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstanceFrom, FromItemCount);
	}
}

void UL1ItemTransferHelper::QuickFromInventory(ALyraCharacter* FromPlayer, ALyraCharacter* ToPlayer, const FIntPoint& FromItemSlotPos)
{
	UL1InventoryManagerComponent* FromInventoryManager = GetCharacterInventoryManager(FromPlayer);
	if (FromInventoryManager == nullptr)
		return;

	UL1InventoryManagerComponent* MyInventoryManager = GetCharacterInventoryManager(ToPlayer);
	UL1EquipmentManagerComponent* MyEquipmentManager = GetCharacterEquipmentManager(ToPlayer);
	if (MyInventoryManager == nullptr || MyEquipmentManager == nullptr)
		return;

	UL1ItemInstance* FromItemInstance = FromInventoryManager->GetItemInstance(FromItemSlotPos);
	if (FromItemInstance == nullptr)
		return;

	if (FromItemInstance->FindFragmentByClass<UL1ItemFragment_Equipable>())
	{
		// 1. [장비]
		// 1-1. [내 인벤토리] -> 내 장비 교체 -> 내 장비 장착 
		// 1-2. [다른 인벤토리] -> 내 장비 교체 -> 내 장비 장착 -> 내 인벤토리

		EEquipmentSlotType ToEquipmentSlotType;
		FIntPoint ToItemSlotPos;
		if (MyEquipmentManager->CanSwapEquipment_Quick(FromInventoryManager, FromItemSlotPos, ToEquipmentSlotType, ToItemSlotPos))
		{
			const int32 FromItemCount = FromInventoryManager->GetItemCount(FromItemSlotPos);
			const int32 ToItemCount = MyEquipmentManager->GetItemCount(ToEquipmentSlotType);

			UL1ItemInstance* RemovedItemInstanceFrom = FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, FromItemCount);
			UL1ItemInstance* RemovedItemInstanceTo = MyEquipmentManager->RemoveEquipment_Unsafe(ToEquipmentSlotType, ToItemCount);
			FromInventoryManager->AddItem_Unsafe(ToItemSlotPos, RemovedItemInstanceTo, ToItemCount);
			MyEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstanceFrom, FromItemCount);
		}
		else
		{
			int32 MovableCount = MyEquipmentManager->CanMoveOrMergeEquipment_Quick(FromInventoryManager, FromItemSlotPos, ToEquipmentSlotType);
			if (MovableCount > 0)
			{
				UL1ItemInstance* RemovedItemInstance = FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, MovableCount);
				MyEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstance, MovableCount);
			}
			else
			{
				if (MyInventoryManager != FromInventoryManager)
				{
					TArray<FIntPoint> OutToItemSlotPoses;
					TArray<int32> OutToItemCounts;
					MovableCount = MyInventoryManager->CanMoveOrMergeItem_Quick(FromInventoryManager, FromItemSlotPos, OutToItemSlotPoses, OutToItemCounts);
					if (MovableCount > 0)
					{
						UL1ItemInstance* RemovedItemInstance = FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, MovableCount);
						for (int32 i = 0; i < OutToItemSlotPoses.Num(); i++)
						{
							MyInventoryManager->AddItem_Unsafe(OutToItemSlotPoses[i], RemovedItemInstance, OutToItemCounts[i]);
						}
						return;
					}
				}
			}
		}
	}
	else
	{
		// 2. [일반 아이템]
		// 2-1. [내 인벤토리] -> X
		// 2-2. [다른 인벤토리] -> 내 인벤토리

		if (MyInventoryManager != FromInventoryManager)
		{
			TArray<FIntPoint> ToItemSlotPoses;
			TArray<int32> ToItemCounts;

			int32 MovableItemCount = MyInventoryManager->CanMoveOrMergeItem_Quick(FromInventoryManager, FromItemSlotPos, ToItemSlotPoses, ToItemCounts);
			if (MovableItemCount > 0)
			{
				UL1ItemInstance* RemovedItemInstance = FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, MovableItemCount);
				for (int32 i = 0; i < ToItemSlotPoses.Num(); i++)
				{
					MyInventoryManager->AddItem_Unsafe(ToItemSlotPoses[i], RemovedItemInstance, ToItemCounts[i]);
				}
			}
		}
	}
}

void UL1ItemTransferHelper::QuickFromEquipment(ALyraCharacter* FromPlayer, ALyraCharacter* ToPlayer, EEquipmentSlotType FromEquipmentSlotType)
{
	UL1EquipmentManagerComponent* FromEquipmentManager = GetCharacterEquipmentManager(FromPlayer);

	if (FromEquipmentManager == nullptr || FromEquipmentSlotType == EEquipmentSlotType::Count)
		return;

	// 1. [내 장비창] -> 내 인벤토리
	// 2. [다른 장비창] -> 내 장비 교체 -> 내 장비 장착 -> 내 인벤토리 

	UL1InventoryManagerComponent* MyInventoryManager = GetCharacterInventoryManager(ToPlayer);
	UL1EquipmentManagerComponent* MyEquipmentManager = GetCharacterEquipmentManager(ToPlayer);
	if (MyInventoryManager == nullptr || MyEquipmentManager == nullptr)
		return;

	if (MyEquipmentManager == FromEquipmentManager)
	{
		TArray<FIntPoint> ToItemSlotPoses;
		TArray<int32> ToItemCounts;

		int32 MovableCount = MyInventoryManager->CanMoveOrMergeItem_Quick(FromEquipmentManager, FromEquipmentSlotType, ToItemSlotPoses, ToItemCounts);
		if (MovableCount > 0)
		{
			UL1ItemInstance* RemovedItemInstance = FromEquipmentManager->RemoveEquipment_Unsafe(FromEquipmentSlotType, MovableCount);
			for (int32 i = 0; i < ToItemSlotPoses.Num(); i++)
			{
				MyInventoryManager->AddItem_Unsafe(ToItemSlotPoses[i], RemovedItemInstance, ToItemCounts[i]);
			}
		}
	}
	else
	{
		EEquipmentSlotType ToEquipmentSlotType;
		if (MyEquipmentManager->CanSwapEquipment_Quick(FromEquipmentManager, FromEquipmentSlotType, ToEquipmentSlotType))
		{
			const int32 FromItemCount = FromEquipmentManager->GetItemCount(FromEquipmentSlotType);
			const int32 ToItemCount = MyEquipmentManager->GetItemCount(ToEquipmentSlotType);

			UL1ItemInstance* RemovedItemInstanceFrom = FromEquipmentManager->RemoveEquipment_Unsafe(FromEquipmentSlotType, FromItemCount);
			UL1ItemInstance* RemovedItemInstanceTo = MyEquipmentManager->RemoveEquipment_Unsafe(ToEquipmentSlotType, ToItemCount);
			FromEquipmentManager->AddEquipment_Unsafe(FromEquipmentSlotType, RemovedItemInstanceTo, ToItemCount);
			MyEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstanceFrom, FromItemCount);
		}
		else
		{
			int32 MovableCount = MyEquipmentManager->CanMoveOrMergeEquipment_Quick(FromEquipmentManager, FromEquipmentSlotType, ToEquipmentSlotType);
			if (MovableCount > 0)
			{
				UL1ItemInstance* RemovedItemInstance = FromEquipmentManager->RemoveEquipment_Unsafe(FromEquipmentSlotType, MovableCount);
				MyEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstance, MovableCount);
			}
			else
			{
				TArray<FIntPoint> ToItemSlotPoses;
				TArray<int32> ToItemCounts;

				MovableCount = MyInventoryManager->CanMoveOrMergeItem_Quick(FromEquipmentManager, FromEquipmentSlotType, ToItemSlotPoses, ToItemCounts);
				if (MovableCount > 0)
				{
					UL1ItemInstance* RemovedItemInstance = FromEquipmentManager->RemoveEquipment_Unsafe(FromEquipmentSlotType, MovableCount);
					for (int32 i = 0; i < ToItemSlotPoses.Num(); i++)
					{
						MyInventoryManager->AddItem_Unsafe(ToItemSlotPoses[i], RemovedItemInstance, ToItemCounts[i]);
					}
				}
			}
		}
	}
}

UL1InventoryManagerComponent* UL1ItemTransferHelper::GetCharacterInventoryManager(ALyraCharacter* LyraCharacter)
{
	UL1InventoryManagerComponent* MyInventoryManager = nullptr;
	if (LyraCharacter)
	{
		MyInventoryManager = LyraCharacter->GetComponentByClass<UL1InventoryManagerComponent>();
	}

	return MyInventoryManager;
}

UL1EquipmentManagerComponent* UL1ItemTransferHelper::GetCharacterEquipmentManager(ALyraCharacter* LyraCharacter)
{
	UL1EquipmentManagerComponent* MyEquipmentManager = nullptr;
	if (LyraCharacter)
	{
		MyEquipmentManager = LyraCharacter->GetComponentByClass<UL1EquipmentManagerComponent>();
	}

	return MyEquipmentManager;
}
