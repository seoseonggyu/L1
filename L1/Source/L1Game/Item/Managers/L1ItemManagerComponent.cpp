#include "Item/Managers/L1ItemManagerComponent.h"
#include "Item/Managers/L1EquipmentManagerComponent.h"
#include "Item/Managers/L1InventoryManagerComponent.h"
#include "Character/LyraCharacter.h"
#include "Network/NetworkUtils.h"
#include "Network/L1NetworkManager.h"
#include "Player/LyraPlayerState.h"
#include "Network/L1BlueprintHelper.h"
#include "Item/L1ItemInstance.h"
#include "Item/Fragments/L1ItemFragment_Equipable.h"
#include "Components/CapsuleComponent.h"
#include "Actors/L1PickupableItemBase.h"
#include "Kismet/GameplayStatics.h"
#include "System/LyraAssetManager.h"

UL1ItemManagerComponent::UL1ItemManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UL1ItemManagerComponent::Check_EquipmentToInventory(UL1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UL1InventoryManagerComponent* ToInventoryManager, const FIntPoint& ToItemSlotPos)
{
	if (FromEquipmentManager == nullptr || ToInventoryManager == nullptr)
		return;

	int32 MovableCount = ToInventoryManager->CanMoveOrMergeItem(FromEquipmentManager, FromEquipmentSlotType, ToItemSlotPos);
	if (MovableCount > 0)
	{
		ALyraCharacter* FromCharacter = Cast<ALyraCharacter>(FromEquipmentManager->GetOwner());
		ALyraCharacter* ToCharacter = Cast<ALyraCharacter>(ToInventoryManager->GetOwner());
		if (FromCharacter == nullptr || ToCharacter == nullptr) return;

		SetNetworkManager();
		if (CachedNetworkManager)
		{
			CachedNetworkManager -> SendPacket_ItemMove(FromCharacter->GetPlayerId(), ToCharacter->GetPlayerId(), FromEquipmentSlotType, EEquipmentSlotType::Count, Protocol::ItemTransferType::Equipment_To_Inventory, FIntPoint(0, 0), ToItemSlotPos, MovableCount);
		}
	}
}

void UL1ItemManagerComponent::Check_InventoryToInventory(UL1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, UL1InventoryManagerComponent* ToInventoryManager, const FIntPoint& ToItemSlotPos)
{
	if (FromInventoryManager == nullptr || ToInventoryManager == nullptr)
		return;

	if (FromInventoryManager == ToInventoryManager && FromItemSlotPos == ToItemSlotPos)
		return;

	int32 MovableCount = ToInventoryManager->CanMoveOrMergeItem(FromInventoryManager, FromItemSlotPos, ToItemSlotPos);
	if (MovableCount > 0)
	{
		ALyraCharacter* FromCharacter = Cast<ALyraCharacter>(FromInventoryManager->GetOwner());
		ALyraCharacter* ToCharacter = Cast<ALyraCharacter>(ToInventoryManager->GetOwner());
		if (FromCharacter == nullptr || ToCharacter == nullptr) return;

		SetNetworkManager();

		if (CachedNetworkManager)
		{
			CachedNetworkManager -> SendPacket_ItemMove(FromCharacter->GetPlayerId(), ToCharacter->GetPlayerId(), EEquipmentSlotType::Count, EEquipmentSlotType::Count, Protocol::ItemTransferType::Inventory_To_Inventory, FromItemSlotPos, ToItemSlotPos, MovableCount);
		}
	}
}

void UL1ItemManagerComponent::Check_InventoryToEquipment(UL1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, UL1EquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType)
{
	if (FromInventoryManager == nullptr || ToEquipmentManager == nullptr)
		return;

	ALyraCharacter* FromCharacter = Cast<ALyraCharacter>(FromInventoryManager->GetOwner());
	ALyraCharacter* ToCharacter = Cast<ALyraCharacter>(ToEquipmentManager->GetOwner());
	if (FromCharacter == nullptr || ToCharacter == nullptr) return;
	SetNetworkManager();

	int32 MovableCount = ToEquipmentManager->CanMoveOrMergeEquipment(FromInventoryManager, FromItemSlotPos, ToEquipmentSlotType);
	if (MovableCount > 0)
	{
		if (CachedNetworkManager)
		{
			CachedNetworkManager->SendPacket_ItemMove(FromCharacter->GetPlayerId(), ToCharacter->GetPlayerId(), EEquipmentSlotType::Count, ToEquipmentSlotType, Protocol::ItemTransferType::Inventory_To_Equipment, FromItemSlotPos, FIntPoint(0, 0), MovableCount);
		}
	}
	else
	{
		FIntPoint ToItemSlotPos;
		if (ToEquipmentManager->CanSwapEquipment(FromInventoryManager, FromItemSlotPos, ToEquipmentSlotType, ToItemSlotPos))
		{
			if (CachedNetworkManager)
			{
				CachedNetworkManager->SendPacket_ItemMove(FromCharacter->GetPlayerId(), ToCharacter->GetPlayerId(), EEquipmentSlotType::Count, ToEquipmentSlotType, Protocol::ItemTransferType::Inventory_To_Equipment, FromItemSlotPos, ToItemSlotPos, MovableCount);
			}
		}
	}
}

void UL1ItemManagerComponent::Check_EquipmentToEquipment(UL1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UL1EquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType)
{
	if (FromEquipmentManager == nullptr || ToEquipmentManager == nullptr)
		return;
	if (FromEquipmentManager == ToEquipmentManager && FromEquipmentSlotType == ToEquipmentSlotType)
		return;

	ALyraCharacter* FromCharacter = Cast<ALyraCharacter>(FromEquipmentManager->GetOwner());
	ALyraCharacter* ToCharacter = Cast<ALyraCharacter>(ToEquipmentManager->GetOwner());
	if (FromCharacter == nullptr || ToCharacter == nullptr) return;
	SetNetworkManager();

	int32 MovableCount = ToEquipmentManager->CanMoveOrMergeEquipment(FromEquipmentManager, FromEquipmentSlotType, ToEquipmentSlotType);
	if (MovableCount > 0)
	{
		if (CachedNetworkManager)
		{
			CachedNetworkManager->SendPacket_ItemMove(FromCharacter->GetPlayerId(), ToCharacter->GetPlayerId(), FromEquipmentSlotType, ToEquipmentSlotType, Protocol::ItemTransferType::Equipment_To_Equipment, FIntPoint(0, 0), FIntPoint(0, 0), MovableCount);
		}
	}
	else if (ToEquipmentManager->CanSwapEquipment(FromEquipmentManager, FromEquipmentSlotType, ToEquipmentSlotType))
	{
		if (CachedNetworkManager)
		{
			CachedNetworkManager->SendPacket_ItemMove(FromCharacter->GetPlayerId(), ToCharacter->GetPlayerId(), FromEquipmentSlotType, ToEquipmentSlotType, Protocol::ItemTransferType::Equipment_To_Equipment, FIntPoint(0, 0), FIntPoint(0, 0), MovableCount);
		}
	}

}

void UL1ItemManagerComponent::Check_QuickFromInventory(ALyraCharacter* ToCharacter, UL1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos)
{
	ALyraCharacter* FromCharacter = Cast<ALyraCharacter>(FromInventoryManager->GetOwner());

	if (FromInventoryManager == nullptr || FromCharacter == nullptr)
		return;

	UL1InventoryManagerComponent* MyInventoryManager = UL1BlueprintHelper::GetCharacterInventoryManager(ToCharacter);
	UL1EquipmentManagerComponent* MyEquipmentManager = UL1BlueprintHelper::GetCharacterEquipmentManager(ToCharacter);

	if (MyInventoryManager == nullptr || MyEquipmentManager == nullptr)
		return;

	UL1ItemInstance* FromItemInstance = FromInventoryManager->GetItemInstance(FromItemSlotPos);
	if (FromItemInstance == nullptr)
		return;

	SetNetworkManager();
	if (CachedNetworkManager)
	{
		CachedNetworkManager->SendPacket_ItemMove(FromCharacter->GetPlayerId(), ToCharacter->GetPlayerId(), EEquipmentSlotType::Count, EEquipmentSlotType::Count, Protocol::ItemTransferType::Quick_From_Inventory, FromItemSlotPos, FIntPoint(0, 0), 0);
	}
}

void UL1ItemManagerComponent::Check_QuickFromEquipment(ALyraCharacter* ToCharacter, UL1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType)
{
	ALyraCharacter* FromCharacter = Cast<ALyraCharacter>(FromEquipmentManager->GetOwner());
	if (FromCharacter == nullptr) return;

	if (FromEquipmentManager == nullptr || FromEquipmentSlotType == EEquipmentSlotType::Count)
		return;

	UL1InventoryManagerComponent* MyInventoryManager = UL1BlueprintHelper::GetCharacterInventoryManager(ToCharacter);
	UL1EquipmentManagerComponent* MyEquipmentManager = UL1BlueprintHelper::GetCharacterEquipmentManager(ToCharacter);
	if (MyInventoryManager == nullptr || MyEquipmentManager == nullptr)
		return;

	SetNetworkManager();
	if (CachedNetworkManager)
	{
		CachedNetworkManager->SendPacket_ItemMove(FromCharacter->GetPlayerId(), ToCharacter->GetPlayerId(), FromEquipmentSlotType, EEquipmentSlotType::Count, Protocol::ItemTransferType::Quick_From_Equipment, FIntPoint(0, 0), FIntPoint(0, 0), 0);
	}
}

void UL1ItemManagerComponent::Check_DropItemFromInventory(UL1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos)
{
	if (FromInventoryManager == nullptr)
		return;

	UL1ItemInstance* FromItemInstance = FromInventoryManager->GetItemInstance(FromItemSlotPos);
	if (FromItemInstance == nullptr)
		return;


	int32 FromItemCount = FromInventoryManager->GetItemCount(FromItemSlotPos);
	if (FromItemCount <= 0)
		return;

	ALyraCharacter* ItemDropCharacter = Cast<ALyraCharacter>(FromInventoryManager->GetOwner());
	if (ItemDropCharacter == nullptr)
		return;

	SetNetworkManager();
	if (CachedNetworkManager)
	{
		CachedNetworkManager->SendPacket_DropItem(ItemDropCharacter->GetPlayerId(), FromItemInstance->GetItemTemplateID(), FromItemSlotPos, FromItemCount, Protocol::ItemTransferType::Drop_From_Inventory, EEquipmentSlotType::Count);
	}
}

void UL1ItemManagerComponent::Check_DropItemFromEquipment(UL1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType)
{
	if (FromEquipmentManager == nullptr || FromEquipmentSlotType == EEquipmentSlotType::Count)
		return;

	UL1ItemInstance* FromItemInstance = FromEquipmentManager->GetItemInstance(FromEquipmentSlotType);
	if (FromItemInstance == nullptr)
		return;

	int32 FromItemCount = FromEquipmentManager->GetItemCount(FromEquipmentSlotType);
	if (FromItemCount <= 0)
		return;

	ALyraCharacter* ItemDropCharacter = Cast<ALyraCharacter>(FromEquipmentManager->GetOwner());
	if (ItemDropCharacter == nullptr)
		return;

	SetNetworkManager();
	if (CachedNetworkManager)
	{
		CachedNetworkManager->SendPacket_DropItem(ItemDropCharacter->GetPlayerId(), FromItemInstance->GetItemTemplateID(), FIntPoint(0, 0), FromItemCount, Protocol::ItemTransferType::Drop_From_Equipment, FromEquipmentSlotType);
	}
}

void UL1ItemManagerComponent::EquipmentToInventory(ALyraCharacter* FromPlayer, EEquipmentSlotType FromEquipmentSlotType, ALyraCharacter* ToPlayer, const FIntPoint& ToItemSlotPos, int32 MovableCount)
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

void UL1ItemManagerComponent::InventoryToInventory(ALyraCharacter* FromPlayer, const FIntPoint& FromItemSlotPos, ALyraCharacter* ToPlayer, const FIntPoint& ToItemSlotPos, int32 MovableCount)
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

void UL1ItemManagerComponent::InventoryToEquipment(ALyraCharacter* FromPlayer, ALyraCharacter* ToPlayer, EEquipmentSlotType ToEquipmentSlotType, const FIntPoint& FromItemSlotPos, const FIntPoint& ToItemSlotPos, int32 MovableCount)
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

void UL1ItemManagerComponent::EquipmentToEquipment(ALyraCharacter* FromPlayer, ALyraCharacter* ToPlayer, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType ToEquipmentSlotType, int32 MovableCount)
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

void UL1ItemManagerComponent::QuickFromInventory(ALyraCharacter* FromPlayer, ALyraCharacter* ToPlayer, const FIntPoint& FromItemSlotPos)
{
	UL1InventoryManagerComponent* FromInventoryManager = UL1BlueprintHelper::GetCharacterInventoryManager(FromPlayer);
	if (FromInventoryManager == nullptr)
		return;

	UL1InventoryManagerComponent* MyInventoryManager = UL1BlueprintHelper::GetCharacterInventoryManager(ToPlayer);
	UL1EquipmentManagerComponent* MyEquipmentManager = UL1BlueprintHelper::GetCharacterEquipmentManager(ToPlayer);
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

void UL1ItemManagerComponent::QuickFromEquipment(ALyraCharacter* FromPlayer, ALyraCharacter* ToPlayer, EEquipmentSlotType FromEquipmentSlotType)
{
	UL1EquipmentManagerComponent* FromEquipmentManager = UL1BlueprintHelper::GetCharacterEquipmentManager(FromPlayer);

	if (FromEquipmentManager == nullptr || FromEquipmentSlotType == EEquipmentSlotType::Count)
		return;

	// 1. [내 장비창] -> 내 인벤토리
	// 2. [다른 장비창] -> 내 장비 교체 -> 내 장비 장착 -> 내 인벤토리 

	UL1InventoryManagerComponent* MyInventoryManager = UL1BlueprintHelper::GetCharacterInventoryManager(ToPlayer);
	UL1EquipmentManagerComponent* MyEquipmentManager = UL1BlueprintHelper::GetCharacterEquipmentManager(ToPlayer);
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

void UL1ItemManagerComponent::TryDropItem(ALyraCharacter* ItemDropCharacter, int32 ItemId, const FIntPoint& ItemSlotPos, int32 ItemCount, Protocol::ItemTransferType ItemTrnsferType, EEquipmentSlotType EquipmentSlotType)
{
	if (ItemDropCharacter == nullptr) return;

	UL1ItemInstance* ItemInstance = NewObject<UL1ItemInstance>();
	ItemInstance->Init(ItemId, EItemRarity::Poor);

	if (TryDrop(ItemDropCharacter, ItemInstance, ItemCount))
	{
		if (ItemTrnsferType == Protocol::Drop_From_Inventory)
		{
			if (UL1InventoryManagerComponent* FromInventoryManager = UL1BlueprintHelper::GetCharacterInventoryManager(ItemDropCharacter))
			{
				FromInventoryManager->RemoveItem_Unsafe(ItemSlotPos, ItemCount);
			}
		}
		else if (ItemTrnsferType == Protocol::Drop_From_Equipment)
		{
			if (UL1EquipmentManagerComponent* FromEquipmentManager = UL1BlueprintHelper::GetCharacterEquipmentManager(ItemDropCharacter))
			{
				FromEquipmentManager->RemoveEquipment_Unsafe(EquipmentSlotType, ItemCount);

			}
		}
	}
}

bool UL1ItemManagerComponent::TryDrop(ALyraCharacter* ItemDropCharacter, UL1ItemInstance* FromItemInstance, int32 FromItemCount)
{
	if (FromItemInstance == nullptr || FromItemCount <= 0)
		return false;

	const float MaxDistance = 100.f;
	const int32 MaxTryCount = 5.f;
	float HalfRadius = ItemDropCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius() / 2.f;
	float QuarterHeight = ItemDropCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() / 2.f;
	TArray<AActor*> ActorsToIgnore = { ItemDropCharacter };

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	for (int32 i = 0; i < MaxTryCount; i++)
	{
		FHitResult HitResult;
		FVector2D RandPoint = FMath::RandPointInCircle(MaxDistance);
		FVector TraceStartLocation = ItemDropCharacter->GetCapsuleComponent()->GetComponentLocation();
		FVector TraceEndLocation = TraceStartLocation + FVector(RandPoint.X, RandPoint.Y, 0.f);

		if (UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), TraceStartLocation, TraceEndLocation, HalfRadius, QuarterHeight, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true))
			continue;

		TSubclassOf<AL1PickupableItemBase> PickupableItemBaseClass = ULyraAssetManager::Get().GetSubclassByName<AL1PickupableItemBase>("PickupableItemBaseClass");
		AL1PickupableItemBase* PickupableItemActor = GetWorld()->SpawnActor<AL1PickupableItemBase>(PickupableItemBaseClass, TraceEndLocation, FRotator::ZeroRotator, SpawnParameters);
		if (PickupableItemActor == nullptr)
			continue;

		FL1PickupInfo PickupInfo;
		PickupInfo.PickupInstance.ItemInstance = FromItemInstance;
		PickupInfo.PickupInstance.ItemCount = FromItemCount;
		PickupableItemActor->SetPickupInfo(PickupInfo);
		return true;
	}
	return false;
}

void UL1ItemManagerComponent::SetNetworkManager()
{
	if (CachedNetworkManager) return;

	if (AController* Controller = GetOwner<AController>())
	{
		if (ALyraPlayerState* PS = Cast<ALyraPlayerState>(Controller->PlayerState))
		{
			CachedNetworkManager = NetworkUtils::GetNetworkManager(PS);
		}
	}
}