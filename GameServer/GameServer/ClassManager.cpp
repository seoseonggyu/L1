#include "pch.h"
#include "ClassManager.h"

void ClassManager::Add(Protocol::CharacterClassType classType, Protocol::CombatInfo info)
{
	// ���� �ʹݿ� ����� �� �ϳ��� �����常 ����ؼ� Lock �ʿ� ����
	_class.insert(make_pair(classType, info));
}

Protocol::CombatInfo ClassManager::GetCombatInfo(Protocol::CharacterClassType classType)
{
	auto it = _class.find(classType);
	if (it != _class.end())
	{
		return it->second;
	}
	return Protocol::CombatInfo();
}
