#pragma once
#include "Protocol.pb.h"

#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
#include "../GradGame.h"
#endif

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16
{
	PKT_C_LOGIN = 1000,
	PKT_S_LOGIN = 1001,
	PKT_C_ENTER_GAME = 1002,
	PKT_S_ENTER_GAME = 1003,
	PKT_C_LEAVE_GAME = 1004,
	PKT_S_LEAVE_GAME = 1005,
	PKT_S_SPAWN = 1006,
	PKT_S_DESPAWN = 1007,
	PKT_S_RESPAWN = 1008,
	PKT_C_MOVE = 1009,
	PKT_S_MOVE = 1010,
	PKT_S_STAT = 1011,
	PKT_S_GAMEREADY = 1012,
	PKT_S_GAMECOUNTDOWN = 1013,
	PKT_S_GAMESTART = 1014,
	PKT_C_FIRE = 1015,
	PKT_S_FIRE = 1016,
	PKT_C_RELOAD = 1017,
	PKT_S_RELOAD = 1018,
	PKT_C_QUICKSLOT = 1019,
	PKT_S_QUICKSLOT = 1020,
	PKT_C_HIT = 1021,
	PKT_S_HIT = 1022,
	PKT_S_SCORE = 1023,
	PKT_S_ENTER_STAGE2 = 1024,
	PKT_S_STAGE2_GAMEREADY = 1025,
	PKT_S_STAGE2_GAMESTART = 1026,
	PKT_S_STAGE2_WINNER = 1027,
	PKT_C_DASH = 1028,
	PKT_S_DASH = 1029,
	PKT_C_SHIELD = 1030,
	PKT_S_SHIELD = 1031,
	PKT_C_SWIRL = 1032,
	PKT_S_SWIRL = 1033,
	PKT_S_STATUENOTIFY = 1034,
	PKT_C_STATUEACTIVE = 1035,
	PKT_S_STATUEACTIVE = 1036,
	PKT_C_STATUEDEACTIVE = 1037,
	PKT_S_STATUEDEACTIVE = 1038,
	PKT_C_STATUECOMPLETE = 1039,
	PKT_S_BUFF = 1040,
};

// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);
bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt);
bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt);
bool Handle_S_LEAVE_GAME(PacketSessionRef& session, Protocol::S_LEAVE_GAME& pkt);
bool Handle_S_SPAWN(PacketSessionRef& session, Protocol::S_SPAWN& pkt);
bool Handle_S_DESPAWN(PacketSessionRef& session, Protocol::S_DESPAWN& pkt);
bool Handle_S_RESPAWN(PacketSessionRef& session, Protocol::S_RESPAWN& pkt);
bool Handle_S_MOVE(PacketSessionRef& session, Protocol::S_MOVE& pkt);
bool Handle_S_STAT(PacketSessionRef& session, Protocol::S_STAT& pkt);
bool Handle_S_GAMEREADY(PacketSessionRef& session, Protocol::S_GAMEREADY& pkt);
bool Handle_S_GAMECOUNTDOWN(PacketSessionRef& session, Protocol::S_GAMECOUNTDOWN& pkt);
bool Handle_S_GAMESTART(PacketSessionRef& session, Protocol::S_GAMESTART& pkt);
bool Handle_S_FIRE(PacketSessionRef& session, Protocol::S_FIRE& pkt);
bool Handle_S_RELOAD(PacketSessionRef& session, Protocol::S_RELOAD& pkt);
bool Handle_S_QUICKSLOT(PacketSessionRef& session, Protocol::S_QUICKSLOT& pkt);
bool Handle_S_HIT(PacketSessionRef& session, Protocol::S_HIT& pkt);
bool Handle_S_SCORE(PacketSessionRef& session, Protocol::S_SCORE& pkt);
bool Handle_S_ENTER_STAGE2(PacketSessionRef& session, Protocol::S_ENTER_STAGE2& pkt);
bool Handle_S_STAGE2_GAMEREADY(PacketSessionRef& session, Protocol::S_STAGE2_GAMEREADY& pkt);
bool Handle_S_STAGE2_GAMESTART(PacketSessionRef& session, Protocol::S_STAGE2_GAMESTART& pkt);
bool Handle_S_STAGE2_WINNER(PacketSessionRef& session, Protocol::S_STAGE2_WINNER& pkt);
bool Handle_S_DASH(PacketSessionRef& session, Protocol::S_DASH& pkt);
bool Handle_S_SHIELD(PacketSessionRef& session, Protocol::S_SHIELD& pkt);
bool Handle_S_SWIRL(PacketSessionRef& session, Protocol::S_SWIRL& pkt);
bool Handle_S_STATUENOTIFY(PacketSessionRef& session, Protocol::S_STATUENOTIFY& pkt);
bool Handle_S_STATUEACTIVE(PacketSessionRef& session, Protocol::S_STATUEACTIVE& pkt);
bool Handle_S_STATUEDEACTIVE(PacketSessionRef& session, Protocol::S_STATUEDEACTIVE& pkt);
bool Handle_S_BUFF(PacketSessionRef& session, Protocol::S_BUFF& pkt);

class ServerPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		GPacketHandler[PKT_S_LOGIN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_LOGIN>(Handle_S_LOGIN, session, buffer, len); };
		GPacketHandler[PKT_S_ENTER_GAME] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_ENTER_GAME>(Handle_S_ENTER_GAME, session, buffer, len); };
		GPacketHandler[PKT_S_LEAVE_GAME] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_LEAVE_GAME>(Handle_S_LEAVE_GAME, session, buffer, len); };
		GPacketHandler[PKT_S_SPAWN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_SPAWN>(Handle_S_SPAWN, session, buffer, len); };
		GPacketHandler[PKT_S_DESPAWN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_DESPAWN>(Handle_S_DESPAWN, session, buffer, len); };
		GPacketHandler[PKT_S_RESPAWN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_RESPAWN>(Handle_S_RESPAWN, session, buffer, len); };
		GPacketHandler[PKT_S_MOVE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_MOVE>(Handle_S_MOVE, session, buffer, len); };
		GPacketHandler[PKT_S_STAT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_STAT>(Handle_S_STAT, session, buffer, len); };
		GPacketHandler[PKT_S_GAMEREADY] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_GAMEREADY>(Handle_S_GAMEREADY, session, buffer, len); };
		GPacketHandler[PKT_S_GAMECOUNTDOWN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_GAMECOUNTDOWN>(Handle_S_GAMECOUNTDOWN, session, buffer, len); };
		GPacketHandler[PKT_S_GAMESTART] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_GAMESTART>(Handle_S_GAMESTART, session, buffer, len); };
		GPacketHandler[PKT_S_FIRE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_FIRE>(Handle_S_FIRE, session, buffer, len); };
		GPacketHandler[PKT_S_RELOAD] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_RELOAD>(Handle_S_RELOAD, session, buffer, len); };
		GPacketHandler[PKT_S_QUICKSLOT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_QUICKSLOT>(Handle_S_QUICKSLOT, session, buffer, len); };
		GPacketHandler[PKT_S_HIT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_HIT>(Handle_S_HIT, session, buffer, len); };
		GPacketHandler[PKT_S_SCORE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_SCORE>(Handle_S_SCORE, session, buffer, len); };
		GPacketHandler[PKT_S_ENTER_STAGE2] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_ENTER_STAGE2>(Handle_S_ENTER_STAGE2, session, buffer, len); };
		GPacketHandler[PKT_S_STAGE2_GAMEREADY] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_STAGE2_GAMEREADY>(Handle_S_STAGE2_GAMEREADY, session, buffer, len); };
		GPacketHandler[PKT_S_STAGE2_GAMESTART] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_STAGE2_GAMESTART>(Handle_S_STAGE2_GAMESTART, session, buffer, len); };
		GPacketHandler[PKT_S_STAGE2_WINNER] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_STAGE2_WINNER>(Handle_S_STAGE2_WINNER, session, buffer, len); };
		GPacketHandler[PKT_S_DASH] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_DASH>(Handle_S_DASH, session, buffer, len); };
		GPacketHandler[PKT_S_SHIELD] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_SHIELD>(Handle_S_SHIELD, session, buffer, len); };
		GPacketHandler[PKT_S_SWIRL] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_SWIRL>(Handle_S_SWIRL, session, buffer, len); };
		GPacketHandler[PKT_S_STATUENOTIFY] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_STATUENOTIFY>(Handle_S_STATUENOTIFY, session, buffer, len); };
		GPacketHandler[PKT_S_STATUEACTIVE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_STATUEACTIVE>(Handle_S_STATUEACTIVE, session, buffer, len); };
		GPacketHandler[PKT_S_STATUEDEACTIVE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_STATUEDEACTIVE>(Handle_S_STATUEDEACTIVE, session, buffer, len); };
		GPacketHandler[PKT_S_BUFF] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_BUFF>(Handle_S_BUFF, session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::C_LOGIN& pkt) { return MakeSendBuffer(pkt, PKT_C_LOGIN); }
	static SendBufferRef MakeSendBuffer(Protocol::C_ENTER_GAME& pkt) { return MakeSendBuffer(pkt, PKT_C_ENTER_GAME); }
	static SendBufferRef MakeSendBuffer(Protocol::C_LEAVE_GAME& pkt) { return MakeSendBuffer(pkt, PKT_C_LEAVE_GAME); }
	static SendBufferRef MakeSendBuffer(Protocol::C_MOVE& pkt) { return MakeSendBuffer(pkt, PKT_C_MOVE); }
	static SendBufferRef MakeSendBuffer(Protocol::C_FIRE& pkt) { return MakeSendBuffer(pkt, PKT_C_FIRE); }
	static SendBufferRef MakeSendBuffer(Protocol::C_RELOAD& pkt) { return MakeSendBuffer(pkt, PKT_C_RELOAD); }
	static SendBufferRef MakeSendBuffer(Protocol::C_QUICKSLOT& pkt) { return MakeSendBuffer(pkt, PKT_C_QUICKSLOT); }
	static SendBufferRef MakeSendBuffer(Protocol::C_HIT& pkt) { return MakeSendBuffer(pkt, PKT_C_HIT); }
	static SendBufferRef MakeSendBuffer(Protocol::C_DASH& pkt) { return MakeSendBuffer(pkt, PKT_C_DASH); }
	static SendBufferRef MakeSendBuffer(Protocol::C_SHIELD& pkt) { return MakeSendBuffer(pkt, PKT_C_SHIELD); }
	static SendBufferRef MakeSendBuffer(Protocol::C_SWIRL& pkt) { return MakeSendBuffer(pkt, PKT_C_SWIRL); }
	static SendBufferRef MakeSendBuffer(Protocol::C_STATUEACTIVE& pkt) { return MakeSendBuffer(pkt, PKT_C_STATUEACTIVE); }
	static SendBufferRef MakeSendBuffer(Protocol::C_STATUEDEACTIVE& pkt) { return MakeSendBuffer(pkt, PKT_C_STATUEDEACTIVE); }
	static SendBufferRef MakeSendBuffer(Protocol::C_STATUECOMPLETE& pkt) { return MakeSendBuffer(pkt, PKT_C_STATUECOMPLETE); }

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}

	template<typename T>
	static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)
	{
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
		SendBufferRef sendBuffer = MakeShared<SendBuffer>(packetSize);
#else
		SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);
#endif

		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;
		pkt.SerializeToArray(&header[1], dataSize);
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};