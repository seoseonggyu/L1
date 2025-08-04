#include "L1ClassData.h"

#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1ClassData)

const UL1ClassData& UL1ClassData::Get()
{
	return ULyraAssetManager::Get().GetClassData();
}

const FL1ClassInfoEntry& UL1ClassData::GetClassInfoEntry(ECharacterClassType ClassType) const
{
	const int32 ClassIndex = (int32)ClassType;
	if (ClassIndex < 0 || ClassIndex >= (int32)ECharacterClassType::Count)
	{
		static FL1ClassInfoEntry EmptyEntry;
		return EmptyEntry;
	}
	
	return ClassInfoEntries[ClassIndex];
}
