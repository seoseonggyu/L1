#include "pch.h"
#include "MonsterManager.h"

void MonsterManager::Add(Protocol::MonsterType monsterType, MonsterInfo monsterInfo)
{
	_monsters.insert(make_pair(monsterType, monsterInfo));
}

MonsterInfo MonsterManager::GetMonster(Protocol::MonsterType monsterType)
{
	auto it = _monsters.find(monsterType);
	if (it != _monsters.end())
	{
		return it->second;
	}
	return MonsterInfo();
}
