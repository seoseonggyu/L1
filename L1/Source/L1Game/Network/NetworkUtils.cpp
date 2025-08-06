#include "NetworkUtils.h"

ECharacterClassType NetworkUtils::ConvertProtoToUEEnum(Protocol::CharacterClassType ProtoType)
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

Protocol::CharacterClassType NetworkUtils::ConvertUEToProtoEnum(ECharacterClassType UEType)
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
