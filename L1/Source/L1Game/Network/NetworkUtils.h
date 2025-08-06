#pragma once

#include "L1Define.h"
#include "Protocol.pb.h"

class NetworkUtils
{
public:
	static ECharacterClassType ConvertProtoToUEEnum(Protocol::CharacterClassType ProtoType);
	static Protocol::CharacterClassType ConvertUEToProtoEnum(ECharacterClassType UEType);
};