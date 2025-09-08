#include "L1MonsterData.h"

#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1MonsterData)

const UL1MonsterData& UL1MonsterData::Get()
{
	return ULyraAssetManager::Get().GetMonsterData();
}

ULyraPawnData* UL1MonsterData::GetPawnData(EMonsterType MonsterType) const
{
	if (MonsterType == EMonsterType::Count)
		return nullptr;
	
	const TObjectPtr<ULyraPawnData>* PawnData = PawnDataMap.Find(MonsterType);
	return PawnData ? PawnData->Get() : nullptr;
}