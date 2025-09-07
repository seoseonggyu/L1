#pragma once

struct SkillRow
{
    int classType = 0;
    int skillType = 0;
    std::wstring skillCode;
    std::wstring skillName;
    std::wstring description;

    std::optional<int>    damage_flat;
    std::optional<float>  stun_sec;
    std::optional<float>  movespeed_pct;
    std::optional<float>  attackspeed_pct;
    std::optional<float>  dmg_reduction_pct;
    std::optional<int>    attack_power_flat;
    std::optional<float>  cooldown_reduction_sec;
    std::optional<float>  lifesteal_pct;
    std::optional<float>  slow_pct;
    std::optional<float>  dot_maxhp_pct_per_sec;
    std::optional<float>  dot_duration_sec;
};

struct SkillKey
{
    using Key = uint32;
    static constexpr Key Make(int classType, int skillType)
    {
        return (static_cast<Key>(classType) << 16) | static_cast<uint16_t>(skillType);
    }
};