#pragma once

#include "SkillTypes.h"

class SkillManager
{
public:
    void Add(const SkillRow& row);
    const SkillRow* Get(Protocol::CharacterClassType classType, Protocol::SkillType skillType) const;
    bool TryGet(Protocol::CharacterClassType classType, Protocol::SkillType skillType, SkillRow& out) const;
    std::vector<SkillRow> GetAllOfClass(Protocol::CharacterClassType classType) const;

    int32 GetSize() { return _skills.size(); }

private:
    std::unordered_map<SkillKey::Key, SkillRow> _skills;
};