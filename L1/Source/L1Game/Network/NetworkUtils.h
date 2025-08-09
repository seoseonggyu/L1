#pragma once

#include "L1Define.h"
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

	static UL1NetworkManager* GetNetworkManager(ALyraPlayerState* LyraPlayerState);
};