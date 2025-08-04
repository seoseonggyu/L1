#pragma once

#include "L1Define.h"
#include "L1ItemFragment_Equipable.h"
#include "L1ItemFragment_Equipable_Armor.generated.h"

UCLASS()
class UL1ItemFragment_Equipable_Armor : public UL1ItemFragment_Equipable
{
	GENERATED_BODY()
	
public:
	UL1ItemFragment_Equipable_Armor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
#if WITH_EDITORONLY_DATA
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
#endif // WITH_EDITORONLY_DATA

public:
	virtual void OnInstanceCreated(UL1ItemInstance* ItemInstance) const override;
	
public:
	UPROPERTY(EditDefaultsOnly)
	EArmorType ArmorType = EArmorType::Count;

	UPROPERTY(EditDefaultsOnly, meta=(EditCondition="ArmorType == EArmorType::Chest", EditConditionHides))
	bool bIsFullBody = false;
	
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<USkeletalMesh> ArmorMesh;

	UPROPERTY(EditDefaultsOnly, meta=(ForceInlineRow))
	TArray<FRarityStatRangeSet> RarityStatRangeSets;
};
