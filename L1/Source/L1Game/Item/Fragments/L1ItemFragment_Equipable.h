#pragma once

#include "L1Define.h"
#include "GameplayTags.h"
#include "Item/L1ItemTemplate.h"
#include "L1ItemFragment_Equipable.generated.h"

class ULyraAbilitySet;
class UL1ItemInstance;

USTRUCT(BlueprintType)
struct FRarityStat
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly)
	EItemRarity Rarity = EItemRarity::Poor;

	UPROPERTY(EditDefaultsOnly)
	int32 Value = 0;
};

USTRUCT(BlueprintType)
struct FRarityStatSet
{
	GENERATED_BODY()

public:
	FRarityStatSet();
	
public:
	UPROPERTY(EditDefaultsOnly, meta=(Categories="SetByCaller"))
	FGameplayTag StatTag;
	
	UPROPERTY(EditDefaultsOnly, EditFixedSize)
	TArray<FRarityStat> RarityStats;
};

USTRUCT(BlueprintType)
struct FRarityStatRange
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly)
	EItemRarity Rarity = EItemRarity::Poor;
	
	UPROPERTY(EditDefaultsOnly)
	int32 MinValue = 0;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxValue = 0;
};

USTRUCT(BlueprintType)
struct FRarityStatRangeSet
{
	GENERATED_BODY()

public:
	FRarityStatRangeSet();
	
public:
	UPROPERTY(EditDefaultsOnly, meta=(Categories="SetByCaller"))
	FGameplayTag StatTag;
	
	UPROPERTY(EditDefaultsOnly, EditFixedSize)
	TArray<FRarityStatRange> RarityStatRanges;
};

UCLASS(Abstract, Const)
class UL1ItemFragment_Equipable : public UL1ItemFragment
{
	GENERATED_BODY()
	
public:
	UL1ItemFragment_Equipable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:
	void AddStatTagStack(UL1ItemInstance* ItemInstance, const TArray<FRarityStatSet>& RarityStatSets) const;
	void AddStatTagStack(UL1ItemInstance* ItemInstance, const TArray<FRarityStatRangeSet>& RarityStatRangeSets) const;

public:
	bool IsEquipableClassType(ECharacterClassType ClassType) const;
	
public:
	EEquipmentType EquipmentType = EEquipmentType::Count;
	
	UPROPERTY(EditDefaultsOnly, meta=(Bitmask, BitmaskEnum="ECharacterClassType"))
	uint32 EquipableClassFlags = ((1 << (uint32)ECharacterClassType::Count) - 1);
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<const ULyraAbilitySet> BaseAbilitySet;
};
