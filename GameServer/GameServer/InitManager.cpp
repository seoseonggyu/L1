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
	ASSERT_CRASH(GDBConnectionPool->Connect(1, L"Driver={SQL Server};Server=Master\\SQLEXPRESS;Database=ServerDb;Trusted_Connection=Yes;"));

	DBConnection* dbConn = GDBConnectionPool->Pop();
	DBSynchronizer dbSync(*dbConn);
	dbSync.Synchronize(L"GameDB.xml");

	{
		WCHAR name[] = L"Seo";

		SP::InsertGold insertGold(*dbConn);
		insertGold.In_Gold(100);
		insertGold.In_Name(name);
		insertGold.In_CreateDate(TIMESTAMP_STRUCT{ 2020, 6, 8 });
		insertGold.Execute();
	}

	{
		SP::GetGold getGold(*dbConn);
		getGold.In_Gold(100);

		int32 id = 0;
		int32 gold = 0;
		WCHAR name[100];
		TIMESTAMP_STRUCT date;

		getGold.Out_Id(OUT id);
		getGold.Out_Gold(OUT gold);
		getGold.Out_Name(OUT name);
		getGold.Out_CreateDate(OUT date);

		getGold.Execute();

		while (getGold.Fetch())
		{
			GConsoleLogger->WriteStdOut(Color::BLUE,
				L"ID[%d] Gold[%d] Name[%s]\n", id, gold, name);
		}
	}
}
