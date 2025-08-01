#pragma once

#include "CoreMinimal.h"
#include "ServerPacketHandler.h"
#include "PacketSession.h"
#include "Protocol.pb.h"
#include "PacketUtils.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "L1NetworkManager.generated.h"

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


public:
	// GameServer
	class FSocket* Socket;
	FString IpAddress = TEXT("127.0.0.1");
	int16 Port = 7777;

	TSharedPtr<class PacketSession> GameServerSession;

public:
};


template <typename T> 
void UL1NetworkManager::SendPacket(T packet) const
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	const SendBufferRef SendBuffer = ServerPacketHandler::MakeSendBuffer(packet);
	GameServerSession->SendPacket(SendBuffer);
}
