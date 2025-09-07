#include "pch.h"
#include "SkillManager.h"

void SkillManager::Add(const SkillRow& row)
{
	const auto k = SkillKey::Make(row.classType, row.skillType);
	_skills.emplace(k, row);
}

const SkillRow* SkillManager::Get(Protocol::CharacterClassType classType, Protocol::SkillType skillType) const
{
    const auto k = SkillKey::Make(static_cast<int>(classType), static_cast<int>(skillType));
    auto it = _skills.find(k);
    if (it == _skills.end()) return nullptr;
    else return &it->second;
}

bool SkillManager::TryGet(Protocol::CharacterClassType classType, Protocol::SkillType skillType, SkillRow& out) const
{
    if (auto p = Get(classType, skillType)) { out = *p; return true; }
    return false;
}

std::vector<SkillRow> SkillManager::GetAllOfClass(Protocol::CharacterClassType classType) const
{
    std::vector<SkillRow> ret;
    for (int st = 1; st <= 2; ++st)
    {
        if (auto p = Get(classType, static_cast<Protocol::SkillType>(st)))
            ret.push_back(*p);
    }
    return ret;
}
