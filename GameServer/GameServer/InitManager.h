#pragma once

extern class ClassManager*			GClassManager;
extern class GameSessionManager*	GSessionManager;
extern class SkillManager*			GSkillManager;
extern class MonsterManager*		GMonsterManager;
extern class ItemManager*			GItemManager;

class InitManager
{
public:
	InitManager();
	~InitManager();

private:
	void LoadFromDB();
	void ConnectAndSync();
	void LoadCharacterClass();
	void LoadSkills();
	void LoadMonsters();
	void LoadItems();
};