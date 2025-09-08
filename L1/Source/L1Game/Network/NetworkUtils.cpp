#include "NetworkUtils.h"

#include "L1NetworkManager.h"
#include "Player/LyraPlayerState.h"

ECharacterClassType NetworkUtils::ConvertClassFromProto(Protocol::CharacterClassType ProtoType)
{
    switch (ProtoType)
    {
    case Protocol::CHARACTER_CLASS_TYPE_FIGHTER:     return ECharacterClassType::Fighter;
    case Protocol::CHARACTER_CLASS_TYPE_SWORDMASTER: return ECharacterClassType::Swordmaster;
    case Protocol::CHARACTER_CLASS_TYPE_BARBARIAN:   return ECharacterClassType::Barbarian;
    case Protocol::CHARACTER_CLASS_TYPE_WIZARD:      return ECharacterClassType::Wizard;
    case Protocol::CHARACTER_CLASS_TYPE_ARCHER:      return ECharacterClassType::Archer;
    default:                                          return ECharacterClassType::Count; // or some invalid value
    }
}

Protocol::CharacterClassType NetworkUtils::ConvertProtoFromClass(ECharacterClassType UEType)
{
    switch (UEType)
    {
    case ECharacterClassType::Fighter:     return Protocol::CHARACTER_CLASS_TYPE_FIGHTER;
    case ECharacterClassType::Swordmaster: return Protocol::CHARACTER_CLASS_TYPE_SWORDMASTER;
    case ECharacterClassType::Barbarian:   return Protocol::CHARACTER_CLASS_TYPE_BARBARIAN;
    case ECharacterClassType::Wizard:      return Protocol::CHARACTER_CLASS_TYPE_WIZARD;
    case ECharacterClassType::Archer:      return Protocol::CHARACTER_CLASS_TYPE_ARCHER;
    default:                               return Protocol::CHARACTER_CLASS_TYPE_NONE;
    }
}

EEquipmentSlotType NetworkUtils::ConvertEquipSlotFromProto(Protocol::EquipmentSlotType ProtoType)
{
    switch (ProtoType)
    {
    case Protocol::Unarmed_LeftHand:        return EEquipmentSlotType::Unarmed_LeftHand;
    case Protocol::Unarmed_RightHand:       return EEquipmentSlotType::Unarmed_RightHand;
    case Protocol::Primary_LeftHand:        return EEquipmentSlotType::Primary_LeftHand;
    case Protocol::Primary_RightHand:       return EEquipmentSlotType::Primary_RightHand;
    case Protocol::Primary_TwoHand:         return EEquipmentSlotType::Primary_TwoHand;
    case Protocol::Secondary_LeftHand:      return EEquipmentSlotType::Secondary_LeftHand;
    case Protocol::Secondary_RightHand:     return EEquipmentSlotType::Secondary_RightHand;
    case Protocol::Secondary_TwoHand:       return EEquipmentSlotType::Secondary_TwoHand;
    case Protocol::Utility_Primary:         return EEquipmentSlotType::Utility_Primary;
    case Protocol::Utility_Secondary:       return EEquipmentSlotType::Utility_Secondary;
    case Protocol::Utility_Tertiary:        return EEquipmentSlotType::Utility_Tertiary;
    case Protocol::Utility_Quaternary:      return EEquipmentSlotType::Utility_Quaternary;
    case Protocol::Helmet:                  return EEquipmentSlotType::Helmet;
    case Protocol::Chest:                   return EEquipmentSlotType::Chest;
    case Protocol::Legs:                    return EEquipmentSlotType::Legs;
    case Protocol::Hands:                   return EEquipmentSlotType::Hands;
    case Protocol::Foot:                    return EEquipmentSlotType::Foot;
    default:                                return EEquipmentSlotType::Count;
    }
}

Protocol::EquipmentSlotType NetworkUtils::ConvertProtoFromEquipSlot(EEquipmentSlotType UEType)
{
    switch (UEType)
    {
    case EEquipmentSlotType::Unarmed_LeftHand:       return Protocol::Unarmed_LeftHand;
    case EEquipmentSlotType::Unarmed_RightHand:      return Protocol::Unarmed_RightHand;
    case EEquipmentSlotType::Primary_LeftHand:       return Protocol::Primary_LeftHand;
    case EEquipmentSlotType::Primary_RightHand:      return Protocol::Primary_RightHand;
    case EEquipmentSlotType::Primary_TwoHand:        return Protocol::Primary_TwoHand;
    case EEquipmentSlotType::Secondary_LeftHand:     return Protocol::Secondary_LeftHand;
    case EEquipmentSlotType::Secondary_RightHand:    return Protocol::Secondary_RightHand;
    case EEquipmentSlotType::Secondary_TwoHand:      return Protocol::Secondary_TwoHand;
    case EEquipmentSlotType::Utility_Primary:        return Protocol::Utility_Primary;
    case EEquipmentSlotType::Utility_Secondary:      return Protocol::Utility_Secondary;
    case EEquipmentSlotType::Utility_Tertiary:       return Protocol::Utility_Tertiary;
    case EEquipmentSlotType::Utility_Quaternary:     return Protocol::Utility_Quaternary;
    case EEquipmentSlotType::Helmet:                 return Protocol::Helmet;
    case EEquipmentSlotType::Chest:                  return Protocol::Chest;
    case EEquipmentSlotType::Legs:                   return Protocol::Legs;
    case EEquipmentSlotType::Hands:                  return Protocol::Hands;
    case EEquipmentSlotType::Foot:                   return Protocol::Foot;
    default:                                         return Protocol::Slot_type_NONE;
    }
}

EEquipState NetworkUtils::ConvertEquipStateFromProto(Protocol::EquipState ProtoType)
{
    switch (ProtoType)
    {
    case Protocol::Equip_State_Unarmed:             return EEquipState::Unarmed;
    case Protocol::Equip_State_Weapon_Primary:      return EEquipState::Weapon_Primary;
    case Protocol::Equip_State_Weapon_Secondary:    return EEquipState::Weapon_Secondary;
    case Protocol::Equip_State_Utility_Primary:     return EEquipState::Utility_Primary;
    case Protocol::Equip_State_Utility_Secondary:   return EEquipState::Utility_Secondary;
    case Protocol::Equip_State_Utility_Tertiary:    return EEquipState::Utility_Tertiary;
    case Protocol::Equip_State_Utility_Quaternary:  return EEquipState::Utility_Quaternary;
    default:                                        return EEquipState::Count;
    }
}

Protocol::EquipState NetworkUtils::ConvertProtoFromEquipState(EEquipState UEType)
{
    switch (UEType)
    {
    case EEquipState::Unarmed:                  return Protocol::Equip_State_Unarmed;
    case EEquipState::Weapon_Primary:           return Protocol::Equip_State_Weapon_Primary;
    case EEquipState::Weapon_Secondary:         return Protocol::Equip_State_Weapon_Secondary;
    case EEquipState::Utility_Primary:          return Protocol::Equip_State_Utility_Primary;
    case EEquipState::Utility_Secondary:        return Protocol::Equip_State_Utility_Secondary;
    case EEquipState::Utility_Tertiary:         return Protocol::Equip_State_Utility_Tertiary;
    case EEquipState::Utility_Quaternary:       return Protocol::Equip_State_Utility_Quaternary;
    default:                                    return Protocol::Equip_State_None;
    }
}

ESkillType NetworkUtils::ConvertSkillTypeFromProto(Protocol::SkillType protoType)
{
    switch (protoType)
    {
    case Protocol::SkillType_Skill_1:   return ESkillType::Skill_1;
    case Protocol::SkillType_Skill_2:   return ESkillType::Skill_2;
    default:                            return ESkillType::Count;
    }
}

FGameplayTag NetworkUtils::ConvertGameplayTagFromSkillType(ESkillType UEType)
{
    switch (UEType)
    {
    case ESkillType::Skill_1:       return L1GameplayTags::GameplayEvent_Skill_1;
    case ESkillType::Skill_2:       return L1GameplayTags::GameplayEvent_Skill_2;
    default:                        return FGameplayTag();
    }
}

Protocol::SkillType NetworkUtils::ConvertProtoFromSkillType(ESkillType UEType)
{
    switch (UEType)
    {
    case ESkillType::Skill_1:       return Protocol::SkillType_Skill_1;
    case ESkillType::Skill_2:       return Protocol::SkillType_Skill_2;
    default:                        return Protocol::SkillType_None;
    }
}

Protocol::SkillType NetworkUtils::ConvertPortoFromGameplayTag(FGameplayTag GameplayTag)
{
    if(GameplayTag == L1GameplayTags::GameplayEvent_Skill_1) return Protocol::SkillType_Skill_1;
    if(GameplayTag == L1GameplayTags::GameplayEvent_Skill_2) return Protocol::SkillType_Skill_2;

    return Protocol::SkillType_None;
}

EMonsterType NetworkUtils::ConvertMonsterTypeFromProto(Protocol::MonsterType protoType)
{
    switch (protoType)
    {
    case Protocol::MONSTER_TYPE_GRUDGE: return EMonsterType::Grudge;
    default:                            return EMonsterType::Count;
    }
}

Protocol::MonsterType NetworkUtils::ConvertProtoFromMonsterType(EMonsterType UEType)
{
    switch (UEType)
    {
    case EMonsterType::Grudge:  return Protocol::MONSTER_TYPE_GRUDGE;
    default:                    return Protocol::MONSTER_TYPE_NONE;
    }
}

UL1NetworkManager* NetworkUtils::GetNetworkManager(ALyraPlayerState* LyraPlayerState)
{
    if (LyraPlayerState)
    {
        if (UWorld* World = LyraPlayerState->GetWorld())
        {
            if (UGameInstance* GameInstance = Cast<UGameInstance>(World->GetGameInstance()))
            {
                UL1NetworkManager* NetworkManager = GameInstance->GetSubsystem<UL1NetworkManager>();
                return NetworkManager;
            }
        }
    }
    return nullptr;
}
