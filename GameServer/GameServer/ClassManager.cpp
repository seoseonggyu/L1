#include "pch.h"
#include "ClassManager.h"

void ClassManager::AddVital(Protocol::CharacterClassType classType, Protocol::VitalInfo info)
{
	// 서버 초반에 실행될 때 하나의 스레드만 사용해서 Lock 필요 없음
	_classVital.insert(make_pair(classType, info));
}

void ClassManager::AddStat(Protocol::CharacterClassType classType, Protocol::StatInfo info)
{
	// 서버 초반에 실행될 때 하나의 스레드만 사용해서 Lock 필요 없음
	_classStat.insert(make_pair(classType, info));
}

Protocol::VitalInfo ClassManager::GetVitalInfo(Protocol::CharacterClassType classType)
{
	auto it = _classVital.find(classType);
	if (it != _classVital.end())
	{
		return it->second;
	}
	return Protocol::VitalInfo();
}

Protocol::StatInfo ClassManager::GetStatInfo(Protocol::CharacterClassType classType)
{
	auto it = _classStat.find(classType);
	if (it != _classStat.end())
	{
		return it->second;
	}
	return Protocol::StatInfo();
}
