#pragma once

class ClassManager
{
public:
	void Add(Protocol::CharacterClassType classType, Protocol::CombatInfo info);
	Protocol::CombatInfo GetCombatInfo(Protocol::CharacterClassType classType);

private:
	unordered_map<Protocol::CharacterClassType, Protocol::CombatInfo> _class;
};

