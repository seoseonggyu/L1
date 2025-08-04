#include "pch.h"
#include "ServerPacketHandler.h"
#include "ServerSession.h"
#include "C_Player.h"
#include <chrono>

using namespace chrono;

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}

bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt)
{
	Protocol::C_ENTER_GAME EnterGamePkt;

	// TODO: ·£´ýÀ¸·Î
	Protocol::PlayerType random_PlayerType = Protocol::PlayerType::PLAYER_TYPE_MICHAEL;
	EnterGamePkt.set_player_type(random_PlayerType);

	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(EnterGamePkt);
	session->Send(sendBuffer);

	return true;
}

bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt)
{
	const Protocol::ObjectInfo& ObjectInfo = pkt.player();
	const uint64 ObjectId = ObjectInfo.object_id();
	g_clients[ObjectId].HandleEnter(static_pointer_cast<ServerSession>(session), pkt.player(), true);

	return true;
}

bool Handle_S_LEAVE_GAME(PacketSessionRef& session, Protocol::S_LEAVE_GAME& pkt)
{
	return false;
}

bool Handle_S_SPAWN(PacketSessionRef& session, Protocol::S_SPAWN& pkt)
{
	return false;
}

bool Handle_S_DESPAWN(PacketSessionRef& session, Protocol::S_DESPAWN& pkt)
{
	return false;
}

bool Handle_S_RESPAWN(PacketSessionRef& session, Protocol::S_RESPAWN& pkt)
{
	return false;
}

bool Handle_S_MOVE(PacketSessionRef& session, Protocol::S_MOVE& pkt)
{
	if (pkt.info().object_id() == 1) {
		auto now_ms = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
		auto d_ms = now_ms - pkt.info().move_time();
		cout << "Delay: " << d_ms << "ms\n";
	}
	return false;
}

bool Handle_S_STAT(PacketSessionRef& session, Protocol::S_STAT& pkt)
{
	return false;
}

bool Handle_S_GAMEREADY(PacketSessionRef& session, Protocol::S_GAMEREADY& pkt)
{
	return false;
}

bool Handle_S_GAMESTART(PacketSessionRef& session, Protocol::S_GAMESTART& pkt)
{
	return false;
}

bool Handle_S_FIRE(PacketSessionRef& session, Protocol::S_FIRE& pkt)
{
	return false;
}

bool Handle_S_RELOAD(PacketSessionRef& session, Protocol::S_RELOAD& pkt)
{
	return false;
}

bool Handle_S_QUICKSLOT(PacketSessionRef& session, Protocol::S_QUICKSLOT& pkt)
{
	return false;
}

bool Handle_S_HIT(PacketSessionRef& session, Protocol::S_HIT& pkt)
{
	return false;
}

bool Handle_S_SCORE(PacketSessionRef& session, Protocol::S_SCORE& pkt)
{
	return false;
}

bool Handle_S_ENTER_STAGE2(PacketSessionRef& session, Protocol::S_ENTER_STAGE2& pkt)
{
	return false;
}

bool Handle_S_STAGE2_GAMEREADY(PacketSessionRef& session, Protocol::S_STAGE2_GAMEREADY& pkt)
{
	return false;
}

bool Handle_S_STAGE2_GAMESTART(PacketSessionRef& session, Protocol::S_STAGE2_GAMESTART& pkt)
{
	return false;
}

bool Handle_S_DASH(PacketSessionRef& session, Protocol::S_DASH& pkt)
{
	return false;
}

bool Handle_S_SHIELD(PacketSessionRef& session, Protocol::S_SHIELD& pkt)
{
	return false;
}

bool Handle_S_SWIRL(PacketSessionRef& session, Protocol::S_SWIRL& pkt)
{
	return false;
}

bool Handle_S_STATUENOTIFY(PacketSessionRef& session, Protocol::S_STATUENOTIFY& pkt)
{
	return false;
}

bool Handle_S_STATUEACTIVE(PacketSessionRef& session, Protocol::S_STATUEACTIVE& pkt)
{
	return false;
}

bool Handle_S_STATUEDEACTIVE(PacketSessionRef& session, Protocol::S_STATUEDEACTIVE& pkt)
{
	return false;
}

bool Handle_S_BUFF(PacketSessionRef& session, Protocol::S_BUFF& pkt)
{
	return false;
}