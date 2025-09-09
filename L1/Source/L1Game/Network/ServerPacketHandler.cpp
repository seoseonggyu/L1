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
	Protocol::C_ENTER_GAME EnterGamePkt;
	EnterGamePkt.set_playerindex(0);
	if (UL1NetworkManager* GameNetwork = GetWorldNetwork(session))
	{
		GameNetwork->SendPacket(EnterGamePkt);
	}
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

bool Handle_S_SELECTCLASS(PacketSessionRef& session, Protocol::S_SELECTCLASS& pkt)
{
	if (UL1NetworkManager* GameNetwork = GetWorldNetwork(session))
	{
		GameNetwork->HandleSelectClass(pkt);
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

bool Handle_S_HIT(PacketSessionRef& session, Protocol::S_HIT& pkt)
{
	if (UL1NetworkManager* GameNetwork = GetWorldNetwork(session))
	{
		GameNetwork->HandleHit(pkt);
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

bool Handle_S_EQUIP_ITEM(PacketSessionRef& session, Protocol::S_EQUIP_ITEM& pkt)
{
	if (UL1NetworkManager* GameNetwork = GetWorldNetwork(session))
	{
		GameNetwork->HandleEquipItem(pkt);
	}
	return true;
}

bool Handle_S_ITEM_DROP(PacketSessionRef& session, Protocol::S_ITEM_DROP& pkt)
{
	if (UL1NetworkManager* GameNetwork = GetWorldNetwork(session))
	{
		GameNetwork->HandleItemDrop(pkt);
	}
	return true;
}

bool Handle_S_SKILL_IMMEDIATE_CAST(PacketSessionRef& session, Protocol::S_SKILL_IMMEDIATE_CAST& pkt)
{
	if (UL1NetworkManager* GameNetwork = GetWorldNetwork(session))
	{
		GameNetwork->HandleSkillImmediateCast(pkt);
	}
	return true;
}