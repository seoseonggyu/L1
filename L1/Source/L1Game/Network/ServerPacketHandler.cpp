#include "ServerPacketHandler.h"
#include "L1NetworkManager.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

UL1NetworkManager* GetWorldNetwork(const PacketSessionRef& Session)
{
	for (auto World : GEngine->GetWorldContexts())
	{
		if (const UGameInstance* GameInstance = World.World()->GetGameInstance())
		{
			if (UL1NetworkManager* NetworkManager = GameInstance->GetSubsystem<UL1NetworkManager>())
			{
				if (NetworkManager->GameServerSession == Session)
					return NetworkManager;
			}
		}
	}
	return nullptr;
}

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	return false;
}

bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt)
{
	// 로비에서 캐릭터 선택해서 인덱스 전송.
	return true;
}

bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt)
{
	if (UL1NetworkManager* GameNetwork = GetWorldNetwork(session))
	{
		GameNetwork->HandleSpawn(pkt);
	}

	return true;
}

bool Handle_S_LEAVE_GAME(PacketSessionRef& session, Protocol::S_LEAVE_GAME& pkt)
{
	if (UL1NetworkManager* GameNetwork = GetWorldNetwork(session))
	{
		// TODO : 게임 종료? 로비로?
	}

	return true;
}

bool Handle_S_SPAWN(PacketSessionRef& session, Protocol::S_SPAWN& pkt)
{
	if (UL1NetworkManager* GameNetwork = GetWorldNetwork(session))
	{
		GameNetwork->HandleSpawn(pkt);
	}

	return true;
}

bool Handle_S_DESPAWN(PacketSessionRef& session, Protocol::S_DESPAWN& pkt)
{
	if (UL1NetworkManager* GameNetwork = GetWorldNetwork(session))
	{
		GameNetwork->HandleDespawn(pkt);
	}

	return true;
}

bool Handle_S_MOVE(PacketSessionRef& session, Protocol::S_MOVE& pkt)
{
	if (UL1NetworkManager* GameNetwork = GetWorldNetwork(session))
	{
		GameNetwork->HandleMove(pkt);
	}

	return true;
}

bool Handle_S_MOVE_ITEM(PacketSessionRef& session, Protocol::S_MOVE_ITEM& pkt)
{
	if (UL1NetworkManager* GameNetwork = GetWorldNetwork(session))
	{
		GameNetwork->HandleMoveItem(pkt);
	}

	return true;
}
