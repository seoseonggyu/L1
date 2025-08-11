#include "pch.h"
#include "ClientPacketHandler.h"
#include "Room.h"
#include "GameSession.h"
#include "ObjectUtils.h"
#include "Player.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;

}

bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt)
{
	Protocol::S_LOGIN loginPkt;
	
	loginPkt.set_success(true);

	SEND_PACKET(loginPkt);
	return true;
}

bool Handle_C_ENTER_GAME(PacketSessionRef& session, Protocol::C_ENTER_GAME& pkt)
{
	PlayerRef player = ObjectUtils::CreatePlayer(static_pointer_cast<GameSession>(session), pkt.class_type());
	GRoom->DoAsync(&Room::HandleEnterPlayer, player);

	return true;
}

bool Handle_C_LEAVE_GAME(PacketSessionRef& session, Protocol::C_LEAVE_GAME& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->_player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->_room.load().lock();
	if (room == nullptr)
		return false;

	room->HandleLeavePlayer(player);

	return true;
}

bool Handle_C_SELECTCLASS(PacketSessionRef& session, Protocol::C_SELECTCLASS& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->_player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->_room.load().lock();
	if (room == nullptr)
		return false;

	room->HandleSelcetClass(pkt);
	return true;
}

bool Handle_C_MOVE(PacketSessionRef& session, Protocol::C_MOVE& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->_player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->_room.load().lock();
	if (room == nullptr)
		return false;

	room->DoAsync(&Room::HandleMove, pkt);

	return true;
}

bool Handle_C_MOVE_ITEM(PacketSessionRef& session, Protocol::C_MOVE_ITEM& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->_player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->_room.load().lock();
	if (room == nullptr)
		return false;

	room->DoAsync(&Room::HandleMoveItem, pkt);
	return false;
}

bool Handle_C_EQUIP_ITEM(PacketSessionRef& session, Protocol::C_EQUIP_ITEM& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->_player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->_room.load().lock();
	if (room == nullptr)
		return false;

	room->DoAsync(&Room::HandleEquipItem, pkt);
	return false;
}