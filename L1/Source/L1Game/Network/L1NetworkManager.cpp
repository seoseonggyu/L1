#include "L1NetworkManager.h"
#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "SocketSubsystem.h"
#include "PacketSession.h"
#include "Protocol.pb.h"
#include "ServerPacketHandler.h"
#include "Kismet/GameplayStatics.h"
#include "Character/LyraCharacter.h"
#include "System/LyraAssetManager.h"
#include "Data/L1NetworkPawnData.h"
#include "Network/NetworkUtils.h"

// SSG:
#include "Data/L1ClassData.h"
#include "Item/Managers/L1EquipmentManagerComponent.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Player/LyraPlayerState.h"

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

// SSG: 
void UL1NetworkManager::SelectClass(ECharacterClassType ClassType)
{
	if (ClassType == ECharacterClassType::Count || ClassType == CharacterClassType)
		return;

	CharacterClassType = ClassType;
	const FL1ClassInfoEntry& ClassEntry = UL1ClassData::Get().GetClassInfoEntry(CharacterClassType);

	auto* PC = UGameplayStatics::GetPlayerController(this, 0);
	ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(PC->GetPawn());
	if (LyraCharacter)
	{
		if (UL1EquipmentManagerComponent* EquipmentManager = LyraCharacter->GetComponentByClass<UL1EquipmentManagerComponent>())
		{
			for (const FL1DefaultItemEntry& DefaultItemEntry : ClassEntry.DefaultItemEntries)
			{
				EquipmentManager->SetEquipment(DefaultItemEntry.EquipmentSlotType, DefaultItemEntry.ItemTemplateClass, DefaultItemEntry.ItemRarity, DefaultItemEntry.ItemCount);
			}
		}
	}
	
	if (LyraCharacter)
	{
		if (ALyraPlayerState* PlayerState = Cast<ALyraPlayerState>(LyraCharacter->GetPlayerState()))
		{
			ULyraAbilitySystemComponent* AbilitySystemComponent = Cast<ULyraAbilitySystemComponent>(PlayerState->GetAbilitySystemComponent());
			AbilitySetGrantedHandles.TakeFromAbilitySystem(AbilitySystemComponent);
			if (ULyraAbilitySet* AbilitySet = ClassEntry.ClassAbilitySet)
			{
				AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, &AbilitySetGrantedHandles, this);
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

	if (IsMine)
	{
		auto* PC = UGameplayStatics::GetPlayerController(this, 0);
		ALyraCharacter* Player = Cast<ALyraCharacter>(PC->GetPawn());
		if (Player == nullptr)
			return;

		Player->SetActorLocation(SpawnLocation);

		Player->SetPlayerInfo(ObjectInfo.pos_info());
		Player->SetDestInfo(ObjectInfo.pos_info());

		MyPlayer = Player;
		Players.Add(ObjectInfo.object_id(), Player);

		// SSG: 임시 적용 캐릭터 클래스
		ECharacterClassType ClassType = NetworkUtils::ConvertProtoToUEEnum(ObjectInfo.character_classtype());
		SelectClass(ClassType);
	}
	else
	{
		const UL1NetworkPawnData& NetworkPawnData = ULyraAssetManager::Get().GetNetworkPawnData();
		ALyraCharacter* Player = Cast<ALyraCharacter>(World->SpawnActor(NetworkPawnData.PawnClass, &SpawnLocation));
		Player->SetPlayerInfo(ObjectInfo.pos_info());
		Player->SetDestInfo(ObjectInfo.pos_info());
		Players.Add(ObjectInfo.object_id(), Player);

		// SSG: 임시 적용
		if (Player)
		{
			ECharacterClassType ClassType = NetworkUtils::ConvertProtoToUEEnum(ObjectInfo.character_classtype());
			const FL1ClassInfoEntry& ClassEntry = UL1ClassData::Get().GetClassInfoEntry(ClassType);
			if (UL1EquipmentManagerComponent* EquipmentManager = Player->GetComponentByClass<UL1EquipmentManagerComponent>())
			{
				for (const FL1DefaultItemEntry& DefaultItemEntry : ClassEntry.DefaultItemEntries)
				{
					EquipmentManager->SetEquipment(DefaultItemEntry.EquipmentSlotType, DefaultItemEntry.ItemTemplateClass, DefaultItemEntry.ItemRarity, DefaultItemEntry.ItemCount);
				}
			}
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