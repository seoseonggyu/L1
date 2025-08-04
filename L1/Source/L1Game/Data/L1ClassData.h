#pragma once

#include "L1Define.h"
#include "L1ClassData.generated.h"

class UL1ItemTemplate;
class ULyraAbilitySet;

USTRUCT(BlueprintType)
struct FL1DefaultItemEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UL1ItemTemplate> ItemTemplateClass;

	UPROPERTY(EditDefaultsOnly)
	EItemRarity ItemRarity = EItemRarity::Poor;

	UPROPERTY(EditDefaultsOnly)
	int32 ItemCount = 1;
};

USTRUCT(BlueprintType)
struct FL1ClassInfoEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FText ClassName;

	UPROPERTY(EditDefaultsOnly)
	TArray<FL1DefaultItemEntry> DefaultItemEntries;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULyraAbilitySet> ClassAbilitySet;
};

UCLASS(BlueprintType, Const)
class UL1ClassData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	static const UL1ClassData& Get();

public:
	const FL1ClassInfoEntry& GetClassInfoEntry(ECharacterClassType ClassType) const;
	
private:
	UPROPERTY(EditDefaultsOnly, meta=(ArraySizeNum="ECharacterClassType"))
	FL1ClassInfoEntry ClassInfoEntries[(int32)ECharacterClassType::Count];
};
