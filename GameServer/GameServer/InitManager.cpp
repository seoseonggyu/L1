#include "pch.h"
#include "InitManager.h"
#include "ClassManager.h"
#include "GameSessionManager.h"
#include "ClientPacketHandler.h"

#include "DBConnectionPool.h"
#include "DBSynchronizer.h"
#include "GenProcedures.h"


ClassManager*		GClassManager = nullptr;
GameSessionManager* GSessionManager = nullptr;

InitManager::InitManager()
{
	GClassManager	= new ClassManager();
	GSessionManager = new GameSessionManager();

	ClientPacketHandler::Init();

	LoadFromDB();
}

InitManager::~InitManager()
{
	delete GClassManager;
	delete GSessionManager;
}

void InitManager::LoadFromDB()
{
	ConnectAndSync();
	LoadCharacterClass();
}

void InitManager::ConnectAndSync()
{
	// SSG: Connect을 1개가 아니라 여러 개로 바꿔야 함(성능 문제)
	ASSERT_CRASH(GDBConnectionPool->Connect(1, L"Driver={SQL Server};Server=Master\\SQLEXPRESS;Database=GameDB;Trusted_Connection=Yes;"));

	DBConnectionPoolGuard dbConn(*GDBConnectionPool);
	DBSynchronizer dbSync(*dbConn);
	dbSync.Synchronize(L"GameDB.xml");
}

void InitManager::LoadCharacterClass()
{
	DBConnectionPoolGuard dbConn(*GDBConnectionPool);
	SP::GetAllCharacterClass getClass(*dbConn);

	int32 classType = 0;
	WCHAR className[40];
	float strength = 0.f;
	float defense = 0.f;
	float vigor = 0.f;
	float agility = 0.f;
	float resourceFulness = 0.f;

	getClass.Out_ClassType(classType);
	getClass.Out_ClassName(className);
	getClass.Out_Strength(strength);
	getClass.Out_Defense(defense);
	getClass.Out_Vigor(vigor);
	getClass.Out_Agility(agility);
	getClass.Out_ResourceFulness(resourceFulness);

	getClass.Execute();

	while (getClass.Fetch())
	{

		GConsoleLogger->WriteStdOut(
			Color::BLUE,
			L"classType[%d] className[%ls] str[%.2f] def[%.2f] vig[%.2f] agi[%.2f] res[%.2f]\n",
			classType, className, strength, defense, vigor, agility, resourceFulness
		);

		Protocol::CombatInfo info;
		info.set_strength(strength);
		info.set_defense(defense);
		info.set_vigor(vigor);
		info.set_agility(agility);
		info.set_resourcefulness(resourceFulness);
		GClassManager->Add(
			static_cast<Protocol::CharacterClassType>(classType),
			info
		);
	}
}
