#pragma once

#include "CoreMinimal.h"
#include "ServerPacketHandler.h"
#include "PacketSession.h"
#include "Protocol.pb.h"
#include "PacketUtils.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "L1Define.h"
#include "Blueprint/UserWidget.h"

#include "L1NetworkManager.generated.h"

class ALyraCharacter;
class UL1EquipmentManagerComponent;
class UL1InventoryManagerComponent;
class UL1ItemInstance;

UCLASS()
class L1GAME_API UL1NetworkManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void ConnectToGameServer();

	UFUNCTION(BlueprintCallable)
	void DisconnectFromGameServer();

	UFUNCTION(BlueprintCallable)
	void HandleRecvPackets();

private:
	bool CheckHandle();
	bool CheckHandleObject(uint64 ObjectId);
	bool CheckHandle(uint64 ObjectId, ALyraCharacter*& OutCharacter);

public:
	void SendPacket(SendBufferRef SendBuffer);

	template<typename T>
	void SendPacket(T packet) const;

	void SendPacket_SelectClass(ECharacterClassType ClassType, ALyraCharacter* Character);
	void SendPacket_ItemMove(int32 FromId, int32 ToId, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType ToEquipmentSlotType, Protocol::ItemTransferType ItemTrnsferType, const FIntPoint& FromItemSlotPos, const FIntPoint& ToItemSlotPos, int32 MovableCount);
	void SendPacket_Hit(int32 AttackId, TArray<int32>& TargetIds, ESkillType SkillType);
	void SendPacket_DropItem(int32 FromId, int32 ItemId, const FIntPoint& FromItemSlotPos, int32 ItemCount, Protocol::ItemTransferType ItemTrnsferType, EEquipmentSlotType EquipmentSlotType);

private:
	UFUNCTION(BlueprintCallable)
	void SelectClass(ECharacterClassType ClassType, ALyraCharacter* Character);

public:
	void HandleSpawn(const Protocol::S_ENTER_GAME& EnterGamePkt);
	void HandleSpawn(const Protocol::S_SPAWN& SpawnPkt);

	void HandleDespawn(uint64 ObjectId);
	void HandleDespawn(const Protocol::S_DESPAWN& DespawnPkt);

	void HandleSelectClass(const Protocol::S_SELECTCLASS& SelectClassPkt);
	void HandleMove(const Protocol::S_MOVE& MovePkt);
	void HandleHit(const Protocol::S_HIT& HitPkt);
	void HandleMoveItem(const Protocol::S_MOVE_ITEM& MoveItemPkt);
	void HandleEquipItem(const Protocol::S_EQUIP_ITEM& EquipItemPkt);
	void HandleItemDrop(const Protocol::S_ITEM_DROP& ItemDropPkt);
	void HandleSkillImmediateCast(const Protocol::S_SKILL_IMMEDIATE_CAST& SkillImmediatePkt);

private:
	void SpawnPlayer(const Protocol::ObjectInfo& ObjectInfo, bool IsMine);
	void SpawnMonster(const Protocol::ObjectInfo& ObjectInfo);

private:
	void SetOverHeadWidget(ALyraCharacter* Object);
	void SetInitObjectInfo(ALyraCharacter* Object, const Protocol::ObjectInfo& ObjectInfo);

private:
	UPROPERTY(EditDefaultsOnly, Category = "L1|Widget")
	TSubclassOf<UUserWidget> OverHeadWidgetClass;

public:
	class FSocket* Socket;
	FString IpAddress = TEXT("127.0.0.1");
	int16 Port = 7777;
	bool bConnected = false;

	TSharedPtr<class PacketSession> GameServerSession;

public:
	UPROPERTY()
	TMap<uint64, ALyraCharacter*> Objects;

	UPROPERTY()
	TObjectPtr<ALyraCharacter> MyPlayer;
};


template <typename T> 
void UL1NetworkManager::SendPacket(T packet) const
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	const SendBufferRef SendBuffer = ServerPacketHandler::MakeSendBuffer(packet);
	GameServerSession->SendPacket(SendBuffer);
}
