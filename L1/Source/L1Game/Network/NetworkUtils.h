#pragma once

#include "L1Define.h"
#include "L1GameplayTags.h"
#include "Protocol.pb.h"

class UL1NetworkManager;
class ALyraPlayerState;

class NetworkUtils
{
public:
	static ECharacterClassType ConvertClassFromProto(Protocol::CharacterClassType ProtoType);
	static Protocol::CharacterClassType ConvertProtoFromClass(ECharacterClassType UEType);
	
	static EEquipmentSlotType ConvertEquipSlotFromProto(Protocol::EquipmentSlotType ProtoType);
	static Protocol::EquipmentSlotType ConvertProtoFromEquipSlot(EEquipmentSlotType UEType);

	static EEquipState ConvertEquipStateFromProto(Protocol::EquipState ProtoType);
	static Protocol::EquipState ConvertProtoFromEquipState(EEquipState UEType);

	static ESkillType ConvertSkillTypeFromProto(Protocol::SkillType protoType);
	static FGameplayTag ConvertGameplayTagFromSkillType(ESkillType UEType);
	static Protocol::SkillType ConvertProtoFromSkillType(ESkillType UEType);
	static Protocol::SkillType ConvertPortoFromGameplayTag(FGameplayTag GameplayTag);

	static EMonsterType ConvertMonsterTypeFromProto(Protocol::MonsterType protoType);
	static Protocol::MonsterType ConvertProtoFromMonsterType(EMonsterType UEType);

	static UL1NetworkManager* GetNetworkManager(ALyraPlayerState* LyraPlayerState);
};