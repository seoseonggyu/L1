#include "L1UIData.h"

#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1UIData)

const UL1UIData& UL1UIData::Get()
{
	return ULyraAssetManager::Get().GetUIData();
}

UTexture2D* UL1UIData::GetEntryRarityTexture(EItemRarity ItemRarity) const
{
	const int32 ItemRarityIndex = (int32)ItemRarity;
	if (ItemRarityIndex < 0 || ItemRarityIndex >= (int32)EItemRarity::Count)
		return nullptr;
	
	return RarityInfoEntries[ItemRarityIndex].EntryTexture;
}

UTexture2D* UL1UIData::GetHoverRarityTexture(EItemRarity ItemRarity) const
{
	const int32 ItemRarityIndex = (int32)ItemRarity;
	if (ItemRarityIndex < 0 || ItemRarityIndex >= (int32)EItemRarity::Count)
		return nullptr;
	
	return RarityInfoEntries[ItemRarityIndex].HoverTexture;
}

FColor UL1UIData::GetRarityColor(EItemRarity ItemRarity) const
{
	const int32 ItemRarityIndex = (int32)ItemRarity;
	if (ItemRarityIndex < 0 || ItemRarityIndex >= (int32)EItemRarity::Count)
		return FColor::Black;
	
	return RarityInfoEntries[ItemRarityIndex].Color;
}

const FL1UIInfo& UL1UIData::GetTagUIInfo(FGameplayTag Tag) const
{
	const FL1UIInfo* UIInfo = TagUIInfos.Find(Tag);
	if (UIInfo == nullptr)
	{
		static FL1UIInfo EmptyInfo;
		return EmptyInfo;
	}

	return *UIInfo;
}
