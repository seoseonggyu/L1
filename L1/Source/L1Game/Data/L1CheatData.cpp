#include "L1CheatData.h"

#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1CheatData)

const UL1CheatData& UL1CheatData::Get()
{
	return ULyraAssetManager::Get().GetCheatData();
}
