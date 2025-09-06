#include "pch.h"
#include "ClassManager.h"

void ClassManager::Add(Protocol::CharacterClassType classType, Protocol::CombatInfo info)
{
	// 서버 초반에 실행될 때 하나의 스레드만 사용해서 Lock 필요 없음
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
