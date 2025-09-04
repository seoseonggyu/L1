#include "pch.h"
#include "ClassManager.h"

void ClassManager::Add(Protocol::CharacterClassType classType)
{
	WRITE_LOCK;
	
	// SSG: DB에서 클래스마다 정보 가지고 와야 함
	Protocol::CombatInfo Info;
	_class.insert(make_pair(classType, Info));
}

void ClassManager::Remove(Protocol::CharacterClassType classType)
{
	WRITE_LOCK;
	_class.erase(classType);
}

Protocol::CombatInfo ClassManager::GetCombatInfo(Protocol::CharacterClassType classType)
{
	WRITE_LOCK;
	auto it = _class.find(classType);
	if (it != _class.end())
	{
		return it->second;
	}
	return Protocol::CombatInfo();
}
