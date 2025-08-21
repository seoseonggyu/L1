#include "L1NetworkManager.h"
#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "SocketSubsystem.h"
#include "PacketSession.h"
#include "Protocol.pb.h"
#include "ServerPacketHandler.h"
#include "Kismet/GameplayStatics.h"
#include "L1NetworkCharacter.h"
#include "System/LyraAssetManager.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Data/L1ClassData.h"
#include "Data/L1NetworkPawnData.h"
#include "Item/L1ItemInstance.h"
#include "Item/Fragments/L1ItemFragment_Equipable.h"
#include "Item/Managers/L1EquipmentManagerComponent.h"
#include "Item/Managers/L1InventoryManagerComponent.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Network/NetworkUtils.h"
#include "L1GameplayTags.h"
#include "System/LyraGameData.h"

void UL1NetworkManager::ConnectToGameServer()
{
	Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("Stream"), TEXT("Client Socket"));
	FIPv4Address Ip;
	FIPv4Address::Parse(IpAddress, Ip);

	TSharedRef<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	InternetAddr->SetIp(Ip.Value);
	InternetAddr->SetPort(Port);


	bConnected = Socket->Connect(*InternetAddr);

	if (bConnected)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Success Coneect To Server")));

		GameServerSession = MakeShared<PacketSession>(Socket);
		GameServerSession->Run();
		
		{
			Protocol::C_LOGIN Pkt;
			SendBufferRef SendBuffer = ServerPacketHandler::MakeSendBuffer(Pkt);
			SendPacket(SendBuffer);
		}
	}
}

void UL1NetworkManager::DisconnectFromGameServer()
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

}

void UL1NetworkManager::HandleRecvPackets()
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	GameServerSession->HandleRecvPackets();
}

void UL1NetworkManager::SendPacket(SendBufferRef SendBuffer)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	GameServerSession->SendPacket(SendBuffer);
}

void UL1NetworkManager::SendPacket_SelectClass(ECharacterClassType ClassType, ALyraCharacter* Character)
{
	if (bConnected) {

		// 서버 연결: 서버한테 캐릭터 직업 선택 보낸다.
		Protocol::CharacterClassType ToClassType = NetworkUtils::ConvertProtoFromClass(ClassType);

		Protocol::C_SELECTCLASS selectClassPkt;
		selectClassPkt.set_object_id(Character->GetPlayerId());
		selectClassPkt.set_class_type(ToClassType);
		SendPacket(selectClassPkt);
	}
	else {
		// 서버랑 연결이 안됐으면 자체적으로 직업 선택한다.
		SelectClass(ClassType, Character);
	}
}

void UL1NetworkManager::SendPacket_ItemMove(int32 FromId, int32 ToId, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType ToEquipmentSlotType, Protocol::ItemTransferType ItemTrnsferType, const FIntPoint& FromItemSlotPos, const FIntPoint& ToItemSlotPos, int32 MovableCount)
{
	Protocol::EquipmentSlotType SendFromEquipmentSlotType = NetworkUtils::ConvertProtoFromEquipSlot(FromEquipmentSlotType);
	Protocol::EquipmentSlotType SendToEquipmentSlotType = NetworkUtils::ConvertProtoFromEquipSlot(ToEquipmentSlotType);

	if (bConnected) {
		Protocol::C_MOVE_ITEM ItemMovePkt;
		ItemMovePkt.set_from_object_id(FromId);
		ItemMovePkt.set_to_object_id(ToId);
		ItemMovePkt.set_from_equipment_slot_type(SendFromEquipmentSlotType);
		ItemMovePkt.set_to_equipment_slot_type(SendToEquipmentSlotType);
		ItemMovePkt.set_item_transfer_type(ItemTrnsferType);
		ItemMovePkt.set_from_slot_pos_x(FromItemSlotPos.X);
		ItemMovePkt.set_from_slot_pos_y(FromItemSlotPos.Y);
		ItemMovePkt.set_to_slot_pos_x(ToItemSlotPos.X);
		ItemMovePkt.set_to_slot_pos_y(ToItemSlotPos.Y);
		ItemMovePkt.set_movable_count(MovableCount);
		SendPacket(ItemMovePkt);
	}
	else {

	}
}

void UL1NetworkManager::SelectClass(ECharacterClassType ClassType, ALyraCharacter* Character)
{
	if (ClassType == ECharacterClassType::Count || ClassType == Character->CharacterClassType)
		return;

	if (!Character) return;

	Character->CharacterClassType = ClassType;
	const FL1ClassInfoEntry& ClassEntry = UL1ClassData::Get().GetClassInfoEntry(Character->CharacterClassType);

	if (Character)
	{
		if (UL1EquipmentManagerComponent* EquipmentManager = Character->GetComponentByClass<UL1EquipmentManagerComponent>())
		{
			for (const FL1DefaultItemEntry& DefaultItemEntry : ClassEntry.DefaultItemEntries)
			{
				EquipmentManager->SetEquipment(DefaultItemEntry.EquipmentSlotType, DefaultItemEntry.ItemTemplateClass, DefaultItemEntry.ItemRarity, DefaultItemEntry.ItemCount);
			}
		}
	}
	
	if (Character)
	{
		ULyraAbilitySystemComponent* AbilitySystemComponent = Cast<ULyraAbilitySystemComponent>(Character->GetAbilitySystemComponent());

		Character->AbilitySetGrantedHandles.TakeFromAbilitySystem(AbilitySystemComponent);
		if (ULyraAbilitySet* AbilitySet = ClassEntry.ClassAbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, &Character->AbilitySetGrantedHandles, this);
		}
	}
}

// TODO: 코드 옮기기
void UL1NetworkManager::Check_EquipmentToInventory(UL1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UL1InventoryManagerComponent* ToInventoryManager, const FIntPoint& ToItemSlotPos)
{
	if (FromEquipmentManager == nullptr || ToInventoryManager == nullptr)
		return;

	int32 MovableCount = ToInventoryManager->CanMoveOrMergeItem(FromEquipmentManager, FromEquipmentSlotType, ToItemSlotPos);
	if (MovableCount > 0)
	{
		ALyraCharacter* FromCharacter = Cast<ALyraCharacter>(FromEquipmentManager->GetOwner());
		ALyraCharacter* ToCharacter = Cast<ALyraCharacter>(ToInventoryManager->GetOwner());
		if (FromCharacter)
		{
			SendPacket_ItemMove(FromCharacter->GetPlayerId(), ToCharacter->GetPlayerId(), FromEquipmentSlotType, EEquipmentSlotType::Count, Protocol::ItemTransferType::Equipment_To_Inventory, FIntPoint(0, 0), ToItemSlotPos, MovableCount);
		}
	}
}

void UL1NetworkManager::Check_InventoryToInventory(UL1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, UL1InventoryManagerComponent* ToInventoryManager, const FIntPoint& ToItemSlotPos)
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
		if (FromCharacter)
		{
			SendPacket_ItemMove(FromCharacter->GetPlayerId(), ToCharacter->GetPlayerId(), EEquipmentSlotType::Count, EEquipmentSlotType::Count, Protocol::ItemTransferType::Inventory_To_Inventory, FromItemSlotPos, ToItemSlotPos, MovableCount);
		}
	}
}

void UL1NetworkManager::Check_InventoryToEquipment(UL1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, UL1EquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType)
{
	if (FromInventoryManager == nullptr || ToEquipmentManager == nullptr)
		return;

	ALyraCharacter* FromCharacter = Cast<ALyraCharacter>(FromInventoryManager->GetOwner());
	ALyraCharacter* ToCharacter = Cast<ALyraCharacter>(ToEquipmentManager->GetOwner());

	int32 MovableCount = ToEquipmentManager->CanMoveOrMergeEquipment(FromInventoryManager, FromItemSlotPos, ToEquipmentSlotType);
	if (MovableCount > 0)
	{
		if (FromCharacter)
		{
			SendPacket_ItemMove(FromCharacter->GetPlayerId(), ToCharacter->GetPlayerId(),  EEquipmentSlotType::Count, ToEquipmentSlotType, Protocol::ItemTransferType::Inventory_To_Equipment, FromItemSlotPos, FIntPoint(0,0), MovableCount);
		}
	}
	else
	{
		FIntPoint ToItemSlotPos;
		if (ToEquipmentManager->CanSwapEquipment(FromInventoryManager, FromItemSlotPos, ToEquipmentSlotType, ToItemSlotPos))
		{
			if (FromCharacter)
			{
				SendPacket_ItemMove(FromCharacter->GetPlayerId(), ToCharacter->GetPlayerId(), EEquipmentSlotType::Count, ToEquipmentSlotType, Protocol::ItemTransferType::Inventory_To_Equipment, FromItemSlotPos, ToItemSlotPos, MovableCount);
			}
		}
	}
}

void UL1NetworkManager::Check_EquipmentToEquipment(UL1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UL1EquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType)
{
	if (FromEquipmentManager == nullptr || ToEquipmentManager == nullptr)
		return;
	if (FromEquipmentManager == ToEquipmentManager && FromEquipmentSlotType == ToEquipmentSlotType)
		return;

	ALyraCharacter* FromCharacter = Cast<ALyraCharacter>(FromEquipmentManager->GetOwner());
	ALyraCharacter* ToCharacter = Cast<ALyraCharacter>(ToEquipmentManager->GetOwner());


	int32 MovableCount = ToEquipmentManager->CanMoveOrMergeEquipment(FromEquipmentManager, FromEquipmentSlotType, ToEquipmentSlotType);
	if (MovableCount > 0)
	{
		if (FromCharacter)
		{
			SendPacket_ItemMove(FromCharacter->GetPlayerId(), ToCharacter->GetPlayerId(), FromEquipmentSlotType, ToEquipmentSlotType, Protocol::ItemTransferType::Equipment_To_Equipment, FIntPoint(0, 0), FIntPoint(0, 0), MovableCount);
		}
	}
	else if (ToEquipmentManager->CanSwapEquipment(FromEquipmentManager, FromEquipmentSlotType, ToEquipmentSlotType))
	{
		if (FromCharacter)
		{
			SendPacket_ItemMove(FromCharacter->GetPlayerId(), ToCharacter->GetPlayerId(), FromEquipmentSlotType, ToEquipmentSlotType, Protocol::ItemTransferType::Equipment_To_Equipment, FIntPoint(0, 0), FIntPoint(0, 0), MovableCount);
		}
	}

}

void UL1NetworkManager::Check_QuickFromInventory(ALyraCharacter* ToCharacter, UL1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos)
{
	ALyraCharacter* FromCharacter = Cast<ALyraCharacter>(FromInventoryManager->GetOwner());
	
	if (FromInventoryManager == nullptr || FromCharacter == nullptr)
		return;

	UL1InventoryManagerComponent* MyInventoryManager = GetCharacterInventoryManager(ToCharacter);
	UL1EquipmentManagerComponent* MyEquipmentManager = GetCharacterEquipmentManager(ToCharacter);

	if (MyInventoryManager == nullptr || MyEquipmentManager == nullptr)
		return;

	UL1ItemInstance* FromItemInstance = FromInventoryManager->GetItemInstance(FromItemSlotPos);
	if (FromItemInstance == nullptr)
		return;

	// TODO: 체크안하고 일단 보냄.. 그냥 쌩으로 보내자!
	SendPacket_ItemMove(FromCharacter->GetPlayerId(), ToCharacter->GetPlayerId(), EEquipmentSlotType::Count, EEquipmentSlotType::Count, Protocol::ItemTransferType::Quick_From_Inventory, FromItemSlotPos, FIntPoint(0, 0), 0);
}

void UL1NetworkManager::Check_QuickFromEquipment(ALyraCharacter* ToCharacter, UL1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType)
{
	ALyraCharacter* FromCharacter = Cast<ALyraCharacter>(FromEquipmentManager->GetOwner());
	if (FromCharacter == nullptr) return;
	
	if (FromEquipmentManager == nullptr || FromEquipmentSlotType == EEquipmentSlotType::Count)
		return;

	UL1InventoryManagerComponent* MyInventoryManager = GetCharacterInventoryManager(ToCharacter);
	UL1EquipmentManagerComponent* MyEquipmentManager = GetCharacterEquipmentManager(ToCharacter);
	if (MyInventoryManager == nullptr || MyEquipmentManager == nullptr)
		return;

	// TODO: 체크안하고 일단 보냄.. 그냥 쌩으로 보내자!
	SendPacket_ItemMove(FromCharacter->GetPlayerId(), ToCharacter->GetPlayerId(), FromEquipmentSlotType, EEquipmentSlotType::Count, Protocol::ItemTransferType::Quick_From_Equipment, FIntPoint(0, 0), FIntPoint(0, 0), 0);
}

void UL1NetworkManager::EquipmentToInventory(ALyraCharacter* FromPlayer, EEquipmentSlotType FromEquipmentSlotType, ALyraCharacter* ToPlayer, const FIntPoint& ToItemSlotPos, int32 MovableCount)
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

void UL1NetworkManager::InventoryToInventory(ALyraCharacter* FromPlayer, const FIntPoint& FromItemSlotPos, ALyraCharacter* ToPlayer, const FIntPoint& ToItemSlotPos, int32 MovableCount)
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

void UL1NetworkManager::InventoryToEquipment(ALyraCharacter* FromPlayer, ALyraCharacter* ToPlayer, EEquipmentSlotType ToEquipmentSlotType, const FIntPoint& FromItemSlotPos, const FIntPoint& ToItemSlotPos, int32 MovableCount)
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

void UL1NetworkManager::EquipmentToEquipment(ALyraCharacter* FromPlayer, ALyraCharacter* ToPlayer, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType ToEquipmentSlotType, int32 MovableCount)
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

void UL1NetworkManager::QuickFromInventory(ALyraCharacter* FromPlayer, ALyraCharacter* ToPlayer, const FIntPoint& FromItemSlotPos)
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

void UL1NetworkManager::QuickFromEquipment(ALyraCharacter* FromPlayer, ALyraCharacter* ToPlayer, EEquipmentSlotType FromEquipmentSlotType)
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

void UL1NetworkManager::HandleSpawn(const Protocol::ObjectInfo& ObjectInfo, bool IsMine)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	auto* World = GetWorld();
	if (World == nullptr)
		return;

	const uint64 ObjectId = ObjectInfo.object_id();
	if (Players.Find(ObjectId) != nullptr)
		return;

	FVector SpawnLocation(ObjectInfo.pos_info().x(), ObjectInfo.pos_info().y(), ObjectInfo.pos_info().z());

	ALyraCharacter* Player = nullptr;
	ECharacterClassType ClassType = NetworkUtils::ConvertClassFromProto(ObjectInfo.character_classtype());


	if (IsMine)
	{
		auto* PC = UGameplayStatics::GetPlayerController(this, 0);
		Player = Cast<ALyraCharacter>(PC->GetPawn());
		if (Player == nullptr)
			return;

		MyPlayer = Player;
	}
	else
	{
		const UL1NetworkPawnData& NetworkPawnData = ULyraAssetManager::Get().GetNetworkPawnData();
		Player = Cast<AL1NetworkCharacter>(World->SpawnActor(NetworkPawnData.PawnClass, &SpawnLocation));
		if (Player == nullptr) return;

		for (ULyraAbilitySet* AbilitySet : NetworkPawnData.AbilitySets)
		{
			if (AbilitySet && Player->GetLyraAbilitySystemComponent())
			{
				AbilitySet->GiveToAbilitySystem(Player->GetLyraAbilitySystemComponent(), nullptr);
			}
		}
	}

	if (Player)
	{
		Players.Add(ObjectInfo.object_id(), Player);
		
		Player->SetActorLocation(SpawnLocation);
		Player->SetPlayerInfo(ObjectInfo.pos_info());
		Player->SetDestInfo(ObjectInfo.pos_info());

		if (UAbilitySystemComponent* ASC = Player->GetAbilitySystemComponent())
		{
			// TODO: 캐릭터 체력 업로드
			float Helath = ObjectInfo.vital_info().max_hp();
			float Mana = ObjectInfo.vital_info().max_mp();

			TSubclassOf<UGameplayEffect> InitialGE = ULyraAssetManager::GetSubclassByPath(ULyraGameData::Get().InitialGameplayEffect_SetByCaller);
			FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(InitialGE, 0, ASC->MakeEffectContext());
			if (EffectSpecHandle.IsValid())
			{
				EffectSpecHandle.Data->SetSetByCallerMagnitude(L1GameplayTags::SetByCaller_InitialAttribute_Health, Helath);
				EffectSpecHandle.Data->SetSetByCallerMagnitude(L1GameplayTags::SetByCaller_InitialAttribute_Mana, Mana);
			}
			
			ASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
		}
	}
}

void UL1NetworkManager::HandleSpawn(const Protocol::S_ENTER_GAME& EnterGamePkt)
{
	HandleSpawn(EnterGamePkt.player(), true);
}

void UL1NetworkManager::HandleSpawn(const Protocol::S_SPAWN& SpawnPkt)
{
	for (auto& Player : SpawnPkt.players())
	{
		HandleSpawn(Player, false);
	}
}

void UL1NetworkManager::HandleDespawn(uint64 ObjectId)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	auto* World = GetWorld();
	if (World == nullptr)
		return;

	ALyraCharacter** FindActor = Players.Find(ObjectId);
	if (FindActor == nullptr)
		return;

	World->DestroyActor(*FindActor);
}

void UL1NetworkManager::HandleDespawn(const Protocol::S_DESPAWN& DespawnPkt)
{
	for (auto& ObjectId : DespawnPkt.object_ids())
	{
		HandleDespawn(ObjectId);
	}
}

void UL1NetworkManager::HandleSelectClass(const Protocol::S_SELECTCLASS& SelectClassPkt)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	auto* World = GetWorld();
	if (World == nullptr)
		return;

	const uint64 ObjectId = SelectClassPkt.object_id();
	ALyraCharacter* FindActor = *(Players.Find(ObjectId));
	if (FindActor == nullptr)
		return;

	ECharacterClassType ClassType = NetworkUtils::ConvertClassFromProto(SelectClassPkt.class_type());
	SelectClass(ClassType, FindActor);
}

void UL1NetworkManager::HandleMove(const Protocol::S_MOVE& MovePkt)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	auto* World = GetWorld();
	if (World == nullptr)
		return;

	const uint64 ObjectId = MovePkt.info().object_id();
	ALyraCharacter** FindActor = Players.Find(ObjectId);
	if (FindActor == nullptr)
		return;

	ALyraCharacter* Player = (*FindActor);
	if (Player == nullptr) return;

	Player->SetDestInfo(MovePkt.info());
}

void UL1NetworkManager::HandleMoveItem(const Protocol::S_MOVE_ITEM& MoveItemPkt)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	auto* World = GetWorld();
	if (World == nullptr)
		return;

	int32 FromId = MoveItemPkt.from_object_id();
	int32 ToId = MoveItemPkt.to_object_id();
	EEquipmentSlotType FromEquipmentSlotType = NetworkUtils::ConvertEquipSlotFromProto(MoveItemPkt.from_equipment_slot_type());
	EEquipmentSlotType ToEquipmentSlotType = NetworkUtils::ConvertEquipSlotFromProto(MoveItemPkt.to_equipment_slot_type());
	Protocol::ItemTransferType TransferType = MoveItemPkt.item_transfer_type();
	const FIntPoint FromItemSlotPos = FIntPoint(MoveItemPkt.from_slot_pos_x(), MoveItemPkt.from_slot_pos_y());
	const FIntPoint ToItemSlotPos = FIntPoint(MoveItemPkt.to_slot_pos_x(), MoveItemPkt.to_slot_pos_y());
	int32 MovableCount = MoveItemPkt.movable_count();

	ALyraCharacter* FromPlayer = *(Players.Find(FromId));
	ALyraCharacter* ToPlayer = *(Players.Find(ToId));
	if (FromPlayer == nullptr || ToPlayer == nullptr) return;
	
	switch (TransferType)
	{
	case Protocol::Equipment_To_Inventory:  EquipmentToInventory(FromPlayer, FromEquipmentSlotType, ToPlayer, ToItemSlotPos, MovableCount); break;
	case Protocol::Equipment_To_Equipment:	EquipmentToEquipment(FromPlayer, ToPlayer, FromEquipmentSlotType, ToEquipmentSlotType, MovableCount); break;
	case Protocol::Inventory_To_Equipment:	InventoryToEquipment(FromPlayer, ToPlayer, ToEquipmentSlotType, FromItemSlotPos, ToItemSlotPos, MovableCount); break;
	case Protocol::Inventory_To_Inventory:	InventoryToInventory(FromPlayer, FromItemSlotPos, ToPlayer, ToItemSlotPos, MovableCount); break;
	case Protocol::Quick_From_Inventory:	QuickFromInventory(FromPlayer, ToPlayer, FromItemSlotPos);
	case Protocol::Quick_From_Equipment:	QuickFromEquipment(FromPlayer, ToPlayer, FromEquipmentSlotType);
	default: break;
	}
}

void UL1NetworkManager::HandleEquipItem(const Protocol::S_EQUIP_ITEM& EquipItemPkt)
{
	uint64 ObjectId = EquipItemPkt.object_id();
	Protocol::EquipState EquipState = EquipItemPkt.equip_state();
	EEquipState UEEquipState = NetworkUtils::ConvertEquipStateFromProto(EquipState);

	ALyraCharacter* FindActor = *(Players.Find(ObjectId));
	if (FindActor == nullptr)
		return;

	FGameplayEventData Payload;
	Payload.EventMagnitude = (int32)UEEquipState;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(FindActor, L1GameplayTags::GameplayEvent_ChangeEquip, Payload);
}

void UL1NetworkManager::HandleSkillImmediateCast(const Protocol::S_SKILL_IMMEDIATE_CAST& SkillImmediatePkt)
{
	uint64 ObjectId = SkillImmediatePkt.object_id();
	ESkillType SkillType = NetworkUtils::ConvertSkillTypeFromProto(SkillImmediatePkt.skill_type());
	FGameplayTag GameplayTag = NetworkUtils::ConvertGameplayTagFromSkillType(SkillType);
	
	ALyraCharacter* FindActor = *(Players.Find(ObjectId));
	if (FindActor == nullptr)
		return;

	FGameplayEventData Payload;
	Payload.EventMagnitude = (int32)SkillType;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(FindActor, GameplayTag, Payload);
}

UL1InventoryManagerComponent* UL1NetworkManager::GetCharacterInventoryManager(ALyraCharacter* LyraCharacter) const
{
	UL1InventoryManagerComponent* MyInventoryManager = nullptr;
	if (LyraCharacter)
	{
		MyInventoryManager = LyraCharacter->GetComponentByClass<UL1InventoryManagerComponent>();
	}

	return MyInventoryManager;
}

UL1EquipmentManagerComponent* UL1NetworkManager::GetCharacterEquipmentManager(ALyraCharacter* LyraCharacter) const
{
	UL1EquipmentManagerComponent* MyEquipmentManager = nullptr;
	if (LyraCharacter)
	{
		MyEquipmentManager = LyraCharacter->GetComponentByClass<UL1EquipmentManagerComponent>();
	}

	return MyEquipmentManager;
}
