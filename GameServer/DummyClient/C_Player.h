#pragma once
#include <chrono>

const int PlayerNum = 2007 + 1;

class C_Player
{
public:
	C_Player();
	virtual ~C_Player();

	void HandleEnter(ServerSessionRef session, const Protocol::ObjectInfo& ObjectInfo, bool IsMine);

public:
	weak_ptr<ServerSession>	session;

	shared_ptr<class Protocol::ObjectInfo> objectInfo;
	shared_ptr<class Protocol::PosInfo> posInfo;
	shared_ptr<class Protocol::StatInfo> StatInfo;


	// Test ฐüทร
	Protocol::PosInfo targetPos;
	bool connected = false;
	chrono::steady_clock::time_point before_ms = chrono::steady_clock::time_point();
};

extern Array<C_Player, PlayerNum> g_clients;