#include "pch.h"
#include "ClassManager.h"

void ClassManager::AddVital(Protocol::CharacterClassType classType, Protocol::VitalInfo info)
{
	// ���� �ʹݿ� ����� �� �ϳ��� �����常 ����ؼ� Lock �ʿ� ����
	_classVital.insert(make_pair(classType, info));
}

void ClassManager::AddStat(Protocol::CharacterClassType classType, Protocol::StatInfo info)
{
	// ���� �ʹݿ� ����� �� �ϳ��� �����常 ����ؼ� Lock �ʿ� ����
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
