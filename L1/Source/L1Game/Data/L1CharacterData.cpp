#include "L1CharacterData.h"

#include "System/LyraAssetManager.h"
#include "UObject/ObjectSaveContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1CharacterData)

const UL1CharacterData& UL1CharacterData::Get()
{
	return ULyraAssetManager::Get().GetCharacterData();
}

#if WITH_EDITOR
void UL1CharacterData::PreSave(FObjectPreSaveContext ObjectSaveContext)
{
	Super::PreSave(ObjectSaveContext);

	DefaultArmorMeshMap.KeySort([](const ECharacterSkinType A, const ECharacterSkinType B)
		{
			return (A < B);
		});
}
#endif // WITH_EDITOR

const FL1DefaultArmorMeshSet& UL1CharacterData::GetDefaultArmorMeshSet(ECharacterSkinType CharacterSkinType) const
{
	if (DefaultArmorMeshMap.Contains(CharacterSkinType) == false)
	{
		static FL1DefaultArmorMeshSet EmptyEntry;
		return EmptyEntry;
	}

	return DefaultArmorMeshMap.FindChecked(CharacterSkinType);
}
