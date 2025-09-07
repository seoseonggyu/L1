#pragma once

extern class ClassManager*			GClassManager;
extern class GameSessionManager*	GSessionManager;
extern class SkillManager*			GSkillManager;

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
};