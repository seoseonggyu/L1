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

	void SendPacket(SendBufferRef SendBuffer);

	template<typename T>
	void SendPacket(T packet) const;

	void SendPacket_SelectClass(ECharacterClassType ClassType, ALyraCharacter* Character);

	UFUNCTION(BlueprintCallable)
	void SelectClass(ECharacterClassType ClassType, ALyraCharacter* Character);

public:
	void HandleSpawn(const Protocol::ObjectInfo& PlayerInfo, bool IsMine);
	void HandleSpawn(const Protocol::S_ENTER_GAME& EnterGamePkt);
	void HandleSpawn(const Protocol::S_SPAWN& SpawnPkt);

	void HandleDespawn(uint64 ObjectId);
	void HandleDespawn(const Protocol::S_DESPAWN& DespawnPkt);

	void HandleMove(const Protocol::S_MOVE& MovePkt);

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
