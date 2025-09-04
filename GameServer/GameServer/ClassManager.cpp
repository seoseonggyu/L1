#include "pch.h"
#include "ClassManager.h"

void ClassManager::Add(Protocol::CharacterClassType classType)
{
	WRITE_LOCK;
	
	// SSG: DB���� Ŭ�������� ���� ������ �;� ��
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
