#pragma once


class CompareUtils
{
public:
    static Protocol::CharacterClassType StringToClassName(const WCHAR* className)
    {
        static const unordered_map<String, Protocol::CharacterClassType> table = {
       {L"Fighter", Protocol::CharacterClassType::CHARACTER_CLASS_TYPE_FIGHTER},
       {L"Wizard", Protocol::CharacterClassType::CHARACTER_CLASS_TYPE_WIZARD},
       {L"Archer", Protocol::CharacterClassType::CHARACTER_CLASS_TYPE_ARCHER},
       {L"Swordmaster", Protocol::CharacterClassType::CHARACTER_CLASS_TYPE_SWORDMASTER},
       {L"Barbarian", Protocol::CharacterClassType::CHARACTER_CLASS_TYPE_BARBARIAN},
        };

        auto it = table.find(className);
        if (it != table.end()) {
            return it->second;
        }
        return Protocol::CharacterClassType::CHARACTER_CLASS_TYPE_NONE;
    }

    static Protocol::MonsterType StringToMonsterName(const WCHAR* monsterName)
    {
        static const unordered_map<String, Protocol::MonsterType> table = {
        {L"Grudge", Protocol::MonsterType::MONSTER_TYPE_GRUDGE},


        };

        auto it = table.find(monsterName);
        if (it != table.end()) {
            return it->second;
        }
        return Protocol::MonsterType::MONSTER_TYPE_NONE;
    }
};