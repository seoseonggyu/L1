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
#include "Data/L1MonsterData.h"
#include "Item/L1ItemInstance.h"
#include "Item/Fragments/L1ItemFragment_Equipable.h"
#include "Item/Managers/L1EquipmentManagerComponent.h"
#include "Item/Managers/L1InventoryManagerComponent.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Network/NetworkUtils.h"
#include "L1GameplayTags.h"
#include "System/LyraGameData.h"
#include "Monster/L1MonsterCharacter.h"
#include "Character/LyraPawnData.h"
#include "L1ItemTransferHelper.h"

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


bool UL1NetworkManager::CheckHandle()
{
	if (Socket == nullptr || GameServerSession == nullptr) return false;
	return (GetWorld() != nullptr) ? true : false;
}

bool UL1NetworkManager::CheckHandleObject(uint64 ObjectId)
{
	if (CheckHandle() == false) return false;
	return (Objects.Find(ObjectId) != nullptr) ? true : false;
}

bool UL1NetworkManager::CheckHandle(uint64 ObjectId, ALyraCharacter*& OutCharacter)
{
	if (CheckHandleObject(ObjectId) == false) return false;

	ALyraCharacter** FindActor = Objects.Find(ObjectId);
	if (FindActor == nullptr || *FindActor == nullptr) return false;

	OutCharacter = *FindActor;
	return true;
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

		Protocol::CharacterClassType ToClassType = NetworkUtils::ConvertProtoFromClass(ClassType);

		Protocol::C_SELECTCLASS selectClassPkt;
		selectClassPkt.set_object_id(Character->GetPlayerId());
		selectClassPkt.set_class_type(ToClassType);
		SendPacket(selectClassPkt);
	}
	else {
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

void UL1NetworkManager::SendPacket_Hit(int32 AttackId, TArray<int32>& TargetIds, ESkillType SkillType)
{
	Protocol::SkillType ProtoSkillType = NetworkUtils::ConvertProtoFromSkillType(SkillType);

	Protocol::C_HIT HitPkt;
	HitPkt.set_attack_object_id(AttackId);
	for (int32& TargetId : TargetIds)
	{
		HitPkt.add_target_object_ids(TargetId);
	}
	HitPkt.set_skill_type(ProtoSkillType);
	SendPacket(HitPkt);
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

// SSG: 코드 옮기기
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

	SendPacket_ItemMove(FromCharacter->GetPlayerId(), ToCharacter->GetPlayerId(), FromEquipmentSlotType, EEquipmentSlotType::Count, Protocol::ItemTransferType::Quick_From_Equipment, FIntPoint(0, 0), FIntPoint(0, 0), 0);
}

void UL1NetworkManager::HandleSpawn(const Protocol::S_ENTER_GAME& EnterGamePkt)
{
	SpawnPlayer(EnterGamePkt.player(), true);
}

void UL1NetworkManager::HandleSpawn(const Protocol::S_SPAWN& SpawnPkt)
{
	for (auto& object: SpawnPkt.objects())
	{
		switch (object.object_type())
		{
		case Protocol::OBJECT_TYPE_PLAYER: 
			SpawnPlayer(object, false);
			break;
		case Protocol::OBJECT_TYPE_MONSTER:
			SpawnMonster(object);
			break;
		default: break;
		}
	}
}

void UL1NetworkManager::HandleDespawn(uint64 ObjectId)
{
	ALyraCharacter* FindActor = nullptr;
	if (CheckHandle(ObjectId, FindActor) == false) return;

	GetWorld()->DestroyActor(FindActor);
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
	auto& ObjectInfo = SelectClassPkt.info();
	ALyraCharacter* FindActor = nullptr;
	if (CheckHandle(ObjectInfo.object_id(), FindActor) == false) return;

	ECharacterClassType ClassType = NetworkUtils::ConvertClassFromProto(ObjectInfo.character_classtype());
	SelectClass(ClassType, FindActor);
	SetInitObjectInfo(FindActor, ObjectInfo);
}

void UL1NetworkManager::HandleMove(const Protocol::S_MOVE& MovePkt)
{
	ALyraCharacter* FindActor = nullptr;
	if (CheckHandle(MovePkt.info().object_id(), FindActor) == false) return;

	FindActor->SetDestInfo(MovePkt.info());
}

void UL1NetworkManager::HandleHit(const Protocol::S_HIT& HitPkt)
{
	const uint64 AttackId = HitPkt.attack_object_id();
	for (const auto& HitTarget : HitPkt.hit_targets())
	{
		const uint64 TargetId = HitTarget.target_object_id();
		float Damage = HitTarget.damage();
		float RemainHp = HitTarget.remaining_hp();

		ALyraCharacter** FindActor = Objects.Find(TargetId);
		if (FindActor == nullptr) continue;
		ALyraCharacter* Player = (*FindActor);
		if (Player == nullptr) continue;

		ULyraAbilitySystemComponent* SourceASC = Player->GetLyraAbilitySystemComponent();
		if (SourceASC == nullptr) continue;

		const TSubclassOf<UGameplayEffect> DamageGE = ULyraAssetManager::GetSubclassByPath(ULyraGameData::Get().DamageGameplayEffect_SetByCaller);
		FGameplayEffectSpecHandle EffectSpecHandle = SourceASC->MakeOutgoingSpec(DamageGE, 0, SourceASC->MakeEffectContext());

		EffectSpecHandle.Data->SetSetByCallerMagnitude(L1GameplayTags::SetByCaller_BaseDamage, Damage);
		SourceASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
		Player->VitalInfo->set_hp(RemainHp);
	}
}

void UL1NetworkManager::HandleMoveItem(const Protocol::S_MOVE_ITEM& MoveItemPkt)
{
	if (CheckHandle() == false) return;

	int32 FromId = MoveItemPkt.from_object_id();
	int32 ToId = MoveItemPkt.to_object_id();
	EEquipmentSlotType FromEquipmentSlotType = NetworkUtils::ConvertEquipSlotFromProto(MoveItemPkt.from_equipment_slot_type());
	EEquipmentSlotType ToEquipmentSlotType = NetworkUtils::ConvertEquipSlotFromProto(MoveItemPkt.to_equipment_slot_type());
	Protocol::ItemTransferType TransferType = MoveItemPkt.item_transfer_type();
	const FIntPoint FromItemSlotPos = FIntPoint(MoveItemPkt.from_slot_pos_x(), MoveItemPkt.from_slot_pos_y());
	const FIntPoint ToItemSlotPos = FIntPoint(MoveItemPkt.to_slot_pos_x(), MoveItemPkt.to_slot_pos_y());
	int32 MovableCount = MoveItemPkt.movable_count();

	ALyraCharacter* FromPlayer = *(Objects.Find(FromId));
	ALyraCharacter* ToPlayer = *(Objects.Find(ToId));
	if (FromPlayer == nullptr || ToPlayer == nullptr) return;

	switch (TransferType)
	{
	case Protocol::Equipment_To_Inventory:  UL1ItemTransferHelper::EquipmentToInventory(FromPlayer, FromEquipmentSlotType, ToPlayer, ToItemSlotPos, MovableCount); break;
	case Protocol::Equipment_To_Equipment:	UL1ItemTransferHelper::EquipmentToEquipment(FromPlayer, ToPlayer, FromEquipmentSlotType, ToEquipmentSlotType, MovableCount); break;
	case Protocol::Inventory_To_Equipment:	UL1ItemTransferHelper::InventoryToEquipment(FromPlayer, ToPlayer, ToEquipmentSlotType, FromItemSlotPos, ToItemSlotPos, MovableCount); break;
	case Protocol::Inventory_To_Inventory:	UL1ItemTransferHelper::InventoryToInventory(FromPlayer, FromItemSlotPos, ToPlayer, ToItemSlotPos, MovableCount); break;
	case Protocol::Quick_From_Inventory:	UL1ItemTransferHelper::QuickFromInventory(FromPlayer, ToPlayer, FromItemSlotPos);
	case Protocol::Quick_From_Equipment:	UL1ItemTransferHelper::QuickFromEquipment(FromPlayer, ToPlayer, FromEquipmentSlotType);
	default: break;
	}
}

void UL1NetworkManager::HandleEquipItem(const Protocol::S_EQUIP_ITEM& EquipItemPkt)
{
	ALyraCharacter* FindActor = nullptr;
	if (CheckHandle(EquipItemPkt.object_id(), FindActor) == false) return;

	Protocol::EquipState EquipState = EquipItemPkt.equip_state();
	EEquipState UEEquipState = NetworkUtils::ConvertEquipStateFromProto(EquipState);

	FGameplayEventData Payload;
	Payload.EventMagnitude = (int32)UEEquipState;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(FindActor, L1GameplayTags::GameplayEvent_ChangeEquip, Payload);
}

void UL1NetworkManager::HandleSkillImmediateCast(const Protocol::S_SKILL_IMMEDIATE_CAST& SkillImmediatePkt)
{
	ALyraCharacter* FindActor = nullptr;
	if (CheckHandle(SkillImmediatePkt.object_id(), FindActor) == false) return;

	ESkillType SkillType = NetworkUtils::ConvertSkillTypeFromProto(SkillImmediatePkt.skill_type());
	FGameplayTag GameplayTag = NetworkUtils::ConvertGameplayTagFromSkillType(SkillType);

	FGameplayEventData Payload;
	Payload.EventMagnitude = (int32)SkillType;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(FindActor, GameplayTag, Payload);
}

void UL1NetworkManager::SpawnPlayer(const Protocol::ObjectInfo& ObjectInfo, bool IsMine)
{
	const uint64 ObjectId = ObjectInfo.object_id();
	if (CheckHandleObject(ObjectId)) return;

	const UL1NetworkPawnData& NetworkPawnData = ULyraAssetManager::Get().GetNetworkPawnData();

	FVector SpawnLocation(ObjectInfo.pos_info().x(), ObjectInfo.pos_info().y(), ObjectInfo.pos_info().z());
	ALyraCharacter* Player = nullptr;
	ECharacterClassType ClassType = NetworkUtils::ConvertClassFromProto(ObjectInfo.character_classtype());

	if (IsMine)
	{
		auto* PC = UGameplayStatics::GetPlayerController(this, 0);
		Player = Cast<ALyraCharacter>(PC->GetPawn());
		if (Player == nullptr) return;

		MyPlayer = Player;
	}
	else
	{
		Player = Cast<AL1NetworkCharacter>(GetWorld()->SpawnActor(NetworkPawnData.PawnClass, &SpawnLocation));
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
		Objects.Add(ObjectId, Player);

		Player->SetActorLocation(SpawnLocation);
		SetInitObjectInfo(Player, ObjectInfo);
		SetOverHeadWidget(Player);
	}
}

void UL1NetworkManager::SpawnMonster(const Protocol::ObjectInfo& ObjectInfo)
{
	const uint64 ObjectId = ObjectInfo.object_id();
	if (CheckHandleObject(ObjectId)) return;

	EMonsterType MonsterType = NetworkUtils::ConvertMonsterTypeFromProto(ObjectInfo.monster_type());
	const UL1MonsterData& MonsterData = ULyraAssetManager::Get().GetMonsterData();
	ULyraPawnData* PawnData = MonsterData.GetPawnData(MonsterType);
	if (PawnData == nullptr) return;

	FVector SpawnLocation(ObjectInfo.pos_info().x(), ObjectInfo.pos_info().y(), ObjectInfo.pos_info().z());
	ALyraCharacter* Monster = nullptr;
	Monster = Cast<AL1MonsterCharacter>(GetWorld()->SpawnActor(PawnData->PawnClass, &SpawnLocation));
	if (Monster == nullptr) return;

	for (ULyraAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		if (AbilitySet && Monster->GetLyraAbilitySystemComponent())
		{
			AbilitySet->GiveToAbilitySystem(Monster->GetLyraAbilitySystemComponent(), nullptr);
		}
	}
	
	Objects.Add(ObjectId, Monster);

	Monster->SetActorLocation(SpawnLocation);
	Monster->SetDestInfo(ObjectInfo.pos_info());
	Monster->SetVitalInfo(ObjectInfo.vital_info());
	
	SetOverHeadWidget(Monster);

	if (UAbilitySystemComponent* ASC = Monster->GetAbilitySystemComponent())
	{
		float Helath = ObjectInfo.vital_info().max_hp();

		TSubclassOf<UGameplayEffect> InitialGE = ULyraAssetManager::GetSubclassByPath(ULyraGameData::Get().InitialGameplayEffect_SetByCaller);
		FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(InitialGE, 0, ASC->MakeEffectContext());
		if (EffectSpecHandle.IsValid())
		{
			EffectSpecHandle.Data->SetSetByCallerMagnitude(L1GameplayTags::SetByCaller_InitialAttribute_Health, Helath);
		}

		ASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
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

void UL1NetworkManager::SetOverHeadWidget(ALyraCharacter* Object)
{
	if (OverHeadWidgetClass == nullptr)
	{
		const UL1NetworkPawnData& NetworkPawnData = ULyraAssetManager::Get().GetNetworkPawnData();
		OverHeadWidgetClass = NetworkPawnData.WidgetClass;
	}
	Object->SetOverHeadWidget(OverHeadWidgetClass);
}

void UL1NetworkManager::SetInitObjectInfo(ALyraCharacter* Object, const Protocol::ObjectInfo& ObjectInfo)
{
	Object->SetDestInfo(ObjectInfo.pos_info());
	Object->SetVitalInfo(ObjectInfo.vital_info());
	Object->SetStatInfo(ObjectInfo.stat_info());

	if (UAbilitySystemComponent* ASC = Object->GetAbilitySystemComponent())
	{
		float Helath = ObjectInfo.vital_info().max_hp();
		float Mana = ObjectInfo.vital_info().max_mp();

		ObjectInfo.stat_info().strength();
		ObjectInfo.stat_info().defense();
		ObjectInfo.stat_info().agility();
		ObjectInfo.stat_info().intelligence();

		TSubclassOf<UGameplayEffect> InitialGE = ULyraAssetManager::GetSubclassByPath(ULyraGameData::Get().InitialGameplayEffect_SetByCaller);
		FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(InitialGE, 0, ASC->MakeEffectContext());
		if (EffectSpecHandle.IsValid())
		{
			EffectSpecHandle.Data->SetSetByCallerMagnitude(L1GameplayTags::SetByCaller_InitialAttribute_Health, Helath);
			EffectSpecHandle.Data->SetSetByCallerMagnitude(L1GameplayTags::SetByCaller_InitialAttribute_Mana, Mana);
			// SSG: 넘겨받은 스탯 업로드
		}

		ASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
};