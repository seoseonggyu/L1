#pragma once

class ClassManager
{
public:
	void Add(Protocol::CharacterClassType classType);
	void Remove(Protocol::CharacterClassType classType);
	Protocol::CombatInfo GetCombatInfo(Protocol::CharacterClassType classType);

private:
	USE_LOCK;
	unordered_map<Protocol::CharacterClassType, Protocol::CombatInfo> _class;
};

