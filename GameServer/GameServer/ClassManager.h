#pragma once

class ClassManager
{
public:
	void AddVital(Protocol::CharacterClassType classType, Protocol::VitalInfo info);
	void AddStat(Protocol::CharacterClassType classType, Protocol::StatInfo info);

	Protocol::VitalInfo GetVitalInfo(Protocol::CharacterClassType classType);
	Protocol::StatInfo GetStatInfo(Protocol::CharacterClassType classType);

private:
	unordered_map<Protocol::CharacterClassType, Protocol::VitalInfo> _classVital;
	unordered_map<Protocol::CharacterClassType, Protocol::StatInfo> _classStat;
};