#pragma once

#include "SkillValue.h"

class SkillManager
{
public:
	void Add(Protocol::CharacterClassType classType, SkillValue value);
	void GetSkills(Protocol::CharacterClassType classType, Vector<SkillValue>& outValues);


	size_t GetSize() { return _skills.size(); }

private:
	multimap<Protocol::CharacterClassType, SkillValue> _skills;
};