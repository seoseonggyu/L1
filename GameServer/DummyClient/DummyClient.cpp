#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "BufferReader.h"
#include "ServerPacketHandler.h"
#include "C_Player.h"
#include "random"
#include <chrono>

using namespace chrono;

default_random_engine dre;
std::uniform_real_distribution<float> uid_X(-4000.f, 100.f);
std::uniform_real_distribution<float> uid_Y(-8500.f, 7700.f);
std::uniform_real_distribution<float> uid_Z(90.f, 90.f);

void Test_Thread()
{
	const float moveSpeed = 600.0f;
	while (true)
	{
		for (int i = 0; i < g_clients.size(); ++i)
		{
			if (!g_clients[i].connected) continue;

			auto& client = g_clients[i];
			auto now = chrono::high_resolution_clock::now();
			chrono::duration<float> elapsed = now - client.before_ms;

			if (elapsed.count() < 0.03f)
				continue;

			client.before_ms = now;
			float deltaTime = elapsed.count();

			// 현재 위치
			float cx = client.posInfo->x();
			float cy = client.posInfo->y();
			float cz = client.posInfo->z();

			// 목표 위치
			float tx = client.targetPos.x();
			float ty = client.targetPos.y();
			float tz = client.targetPos.z();

			// 방향 벡터 계산
			float dx = tx - cx;
			float dy = ty - cy;
			float dz = tz - cz;
			float distance = sqrt(dx * dx + dy * dy + dz * dz);

			if (distance < 10.0f) // 목표 도착
			{
				// 새 랜덤 목표 설정
				client.targetPos.set_x(uid_X(dre));
				client.targetPos.set_y(uid_Y(dre));
				client.targetPos.set_z(uid_Z(dre));
				continue;
			}

			// 정규화된 방향
			float nx = dx / distance;
			float ny = dy / distance;
			float nz = dz / distance;

			// 이동 거리 = 속도 * 시간
			float moveDist = moveSpeed * deltaTime;
			if (moveDist > distance) moveDist = distance;

			// 새 위치
			cx += nx * moveDist;
			cy += ny * moveDist;
			cz += nz * moveDist;

			client.posInfo->set_x(cx);
			client.posInfo->set_y(cy);
			client.posInfo->set_z(cz);

			// 이동 패킷 생성 및 전송
			Protocol::C_MOVE movePkt;
			Protocol::PosInfo* info = movePkt.mutable_info();
			info->CopyFrom(*client.posInfo);
			info->set_move_state(Protocol::MoveState::MOVE_STATE_RUN);
			info->set_move_time(static_cast<uint32>(duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count()));

			auto sendBuffer = ServerPacketHandler::MakeSendBuffer(movePkt);
			if (auto session = client.session.lock())
				session->Send(sendBuffer);
		}
	}
}

int main()
{
	ServerPacketHandler::Init();

	this_thread::sleep_for(5s);

	ClientServiceRef service = MakeShared<ClientService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<ServerSession>,
		PlayerNum - 1);


	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 2; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}

	thread test_thread = thread{ Test_Thread };

	test_thread.join();
	GThreadManager->Join();
}