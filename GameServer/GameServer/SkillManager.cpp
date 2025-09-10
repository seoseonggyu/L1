#include "pch.h"
#include "SkillManager.h"

void SkillManager::Add(Protocol::CharacterClassType classType, SkillValue value)
{
	_skills.insert(make_pair(classType, value));
}

void SkillManager::GetSkills(Protocol::CharacterClassType classType, Vector<SkillValue>& outValues)
{
	auto rangeIter = _skills.equal_range(classType);

	for (auto iter = rangeIter.first; iter != rangeIter.second; ++iter)
	{
		outValues.push_back(iter->second);
	}
}