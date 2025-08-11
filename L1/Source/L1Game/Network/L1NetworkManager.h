#pragma once

#include "CoreMinimal.h"
#include "ServerPacketHandler.h"
#include "PacketSession.h"
#include "Protocol.pb.h"
#include "PacketUtils.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "L1Define.h"

#include "L1NetworkManager.generated.h"

class ALyraCharacter;
class UL1EquipmentManagerComponent;
class UL1InventoryManagerComponent;

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

public:
	void SendPacket(SendBufferRef SendBuffer);

	template<typename T>
	void SendPacket(T packet) const;

	void SendPacket_SelectClass(ECharacterClassType ClassType, ALyraCharacter* Character);
	void SendPacket_ItemMove(int32 FromId, int32 ToId, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType ToEquipmentSlotType, Protocol::ItemTransferType ItemTrnsferType, const FIntPoint& FromItemSlotPos, const FIntPoint& ToItemSlotPos, int32 MovableCount);

private:
	UFUNCTION(BlueprintCallable)
	void SelectClass(ECharacterClassType ClassType, ALyraCharacter* Character);

public:
	void Check_EquipmentToInventory(UL1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UL1InventoryManagerComponent* ToInventoryManager, const FIntPoint& ToItemSlotPos);
	void Check_InventoryToInventory(UL1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, UL1InventoryManagerComponent* ToInventoryManager, const FIntPoint& ToItemSlotPos);
	void Check_InventoryToEquipment(UL1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, UL1EquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType);
	void Check_EquipmentToEquipment(UL1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UL1EquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType);
	void Check_QuickFromInventory(ALyraCharacter* ToCharacter, UL1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos);
	void Check_QuickFromEquipment(ALyraCharacter* ToCharacter, UL1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType);

private:
	UFUNCTION(BlueprintCallable)
	void EquipmentToInventory(ALyraCharacter* FromPlayer, EEquipmentSlotType FromEquipmentSlotType, ALyraCharacter* ToPlayer, const FIntPoint& ToItemSlotPos, int32 MovavleCount);
	UFUNCTION(BlueprintCallable)
	void InventoryToInventory(ALyraCharacter* FromPlayer, const FIntPoint& FromItemSlotPos, ALyraCharacter* ToPlayer, const FIntPoint& ToItemSlotPos, int32 MovavleCount);
	UFUNCTION(BlueprintCallable)
	void InventoryToEquipment(ALyraCharacter* FromPlayer, ALyraCharacter* ToPlayer, EEquipmentSlotType ToEquipmentSlotType, const FIntPoint& FromItemSlotPos, const FIntPoint& ToItemSlotPos, int32 MovableCount);
	UFUNCTION(BlueprintCallable)
	void EquipmentToEquipment(ALyraCharacter* FromPlayer, ALyraCharacter* ToPlayer, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType ToEquipmentSlotType, int32 MovableCount);
	UFUNCTION(BlueprintCallable)
	void QuickFromInventory(ALyraCharacter* FromPlayer, ALyraCharacter* ToPlayer, const FIntPoint& FromItemSlotPos);
	UFUNCTION(BlueprintCallable)
	void QuickFromEquipment(ALyraCharacter* FromPlayer, ALyraCharacter* ToPlayer, EEquipmentSlotType FromEquipmentSlotType);

public:
	void HandleSpawn(const Protocol::ObjectInfo& PlayerInfo, bool IsMine);
	void HandleSpawn(const Protocol::S_ENTER_GAME& EnterGamePkt);
	void HandleSpawn(const Protocol::S_SPAWN& SpawnPkt);

	void HandleDespawn(uint64 ObjectId);
	void HandleDespawn(const Protocol::S_DESPAWN& DespawnPkt);

	void HandleSelectClass(const Protocol::S_SELECTCLASS& SelectClassPkt);
	void HandleMove(const Protocol::S_MOVE& MovePkt);
	void HandleMoveItem(const Protocol::S_MOVE_ITEM& MoveItemPkt);
	void HandleEquipItem(const Protocol::S_EQUIP_ITEM& EquipItemPkt);

private:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UL1InventoryManagerComponent* GetCharacterInventoryManager(ALyraCharacter* LyraCharacter) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UL1EquipmentManagerComponent* GetCharacterEquipmentManager(ALyraCharacter* LyraCharacter) const;

public:
	class FSocket* Socket;
	FString IpAddress = TEXT("127.0.0.1");
	int16 Port = 7777;
	bool bConnected = false;

	TSharedPtr<class PacketSession> GameServerSession;

public:
	UPROPERTY()
	TMap<uint64, ALyraCharacter*> Players;

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
