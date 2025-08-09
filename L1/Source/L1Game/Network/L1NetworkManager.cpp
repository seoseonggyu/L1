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
#include "Data/L1ClassData.h"
#include "Data/L1NetworkPawnData.h"
#include "Item/Managers/L1EquipmentManagerComponent.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Network/NetworkUtils.h"

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
		Protocol::CharacterClassType ToClassType = NetworkUtils::ConvertUEToProtoEnum(ClassType);

		Protocol::C_ENTER_GAME EnterGamePkt;
		EnterGamePkt.set_class_type(ToClassType); // SSG: 캐릭터 직업 선택 임시로
		EnterGamePkt.set_playerindex(0);
		SendPacket(EnterGamePkt);
	}
	else {
		// 서버랑 연결이 안됐으면 자체적으로 직업 선택한다.
		SelectClass(ClassType, Character);
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
	ECharacterClassType ClassType = NetworkUtils::ConvertProtoToUEEnum(ObjectInfo.character_classtype());


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
	}

	if (Player)
	{
		Players.Add(ObjectInfo.object_id(), Player);
		
		Player->SetActorLocation(SpawnLocation);
		Player->SetPlayerInfo(ObjectInfo.pos_info());
		Player->SetDestInfo(ObjectInfo.pos_info());

		SelectClass(ClassType, Player);
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