#include "pch.h"
#include "C_Player.h"
#include "ServerSession.h"

array<C_Player, PlayerNum> g_clients;

C_Player::C_Player()
{
	objectInfo = make_shared<Protocol::ObjectInfo>();
	posInfo = make_shared<Protocol::PosInfo>();
	StatInfo = make_shared<Protocol::StatInfo>();
}

C_Player::~C_Player()
{
}

void C_Player::HandleEnter(ServerSessionRef Session, const Protocol::ObjectInfo& ObjectInfo, bool IsMine)
{
	objectInfo->CopyFrom(ObjectInfo);
	posInfo->CopyFrom(ObjectInfo.pos_info());
	StatInfo->CopyFrom(ObjectInfo.stat_info());
	
	targetPos.CopyFrom(ObjectInfo.pos_info());

	session = Session;
	connected = true;
	before_ms = chrono::high_resolution_clock::now(); // 시작 시간 저장
}