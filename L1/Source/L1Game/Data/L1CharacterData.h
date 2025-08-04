#pragma once

#include "L1Define.h"
#include "L1CharacterData.generated.h"

USTRUCT(BlueprintType)
struct FL1DefaultArmorMeshSet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UMaterialInterface> UpperBodySkinMaterial;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UMaterialInterface> LowerBodySkinMaterial;

public:
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<USkeletalMesh> HeadDefaultMesh;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<USkeletalMesh> HeadSecondaryMesh;

public:
	UPROPERTY(EditDefaultsOnly, meta = (ArraySizeEnum = "EArmorType"))
	TSoftObjectPtr<USkeletalMesh> DefaultMeshEntries[(int32)EArmorType::Count];

	UPROPERTY(EditDefaultsOnly, meta = (ArraySizeEnum = "EArmorType"))
	TSoftObjectPtr<USkeletalMesh> SecondaryMeshEntries[(int32)EArmorType::Count];
};

UCLASS(BlueprintType, Const)
class UL1CharacterData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	static const UL1CharacterData& Get();

public:
#if WITH_EDITOR
	virtual void PreSave(FObjectPreSaveContext ObjectSaveContext) override;
#endif // WITH_EDITOR

public:
	const FL1DefaultArmorMeshSet& GetDefaultArmorMeshSet(ECharacterSkinType CharacterSkinType) const;

private:
	UPROPERTY(EditDefaultsOnly)
	TMap<ECharacterSkinType, FL1DefaultArmorMeshSet> DefaultArmorMeshMap;
};
