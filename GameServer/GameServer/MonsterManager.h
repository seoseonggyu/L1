#pragma once

struct MonsterInfo
{
public:
	int32 monsterID = 0;
	String monsterName;
	float hp;
	float damage;
};


class MonsterManager
{
public:
	void Add(Protocol::MonsterType monsterType, MonsterInfo monsterInfo);

	MonsterInfo GetMonster(Protocol::MonsterType monsterType);

private:
	unordered_map<Protocol::MonsterType, MonsterInfo> _monsters;
};

