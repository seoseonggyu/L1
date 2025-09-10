#pragma once

struct SkillValue
{
    int32 skillID = 0;
    String className;
    String skillType;
    String skillName;
    String description;

    optional<int>    damage_flat;
    optional<float>  stun_sec;
    optional<float>  movespeed_pct;
    optional<float>  attackspeed_pct;
    optional<float>  dmg_reduction_pct;
    optional<int>    attack_power_flat;
    optional<float>  cooldown_reduction_sec;
    optional<float>  lifesteal_pct;
    optional<float>  slow_pct;
    optional<float>  dot_maxhp_pct_per_sec;
    optional<float>  dot_duration_sec;
};