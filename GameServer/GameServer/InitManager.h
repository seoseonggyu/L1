#pragma once

extern class ClassManager*			GClassManager;
extern class GameSessionManager*	GSessionManager;

class InitManager
{
public:
	InitManager();
	~InitManager();

private:
	void LoadFromDB();
	void ConnectAndSync();
	void LoadCharacterClass();
};