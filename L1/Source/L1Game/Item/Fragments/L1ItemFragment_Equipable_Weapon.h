#pragma once

#include "L1Define.h"
#include "L1ItemFragment_Equipable_Attachment.h"
#include "L1ItemFragment_Equipable_Weapon.generated.h"

UCLASS()
class UL1ItemFragment_Equipable_Weapon : public UL1ItemFragment_Equipable_Attachment
{
	GENERATED_BODY()
	
public:
	UL1ItemFragment_Equipable_Weapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
#if WITH_EDITORONLY_DATA
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
#endif // WITH_EDITORONLY_DATA
	
public:
	virtual void OnInstanceCreated(UL1ItemInstance* ItemInstance) const override;
	
public:
	UPROPERTY(EditDefaultsOnly)
	EWeaponType WeaponType = EWeaponType::Count;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> AttackSwingSound;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<const ULyraAbilitySet>> SkillAbilitySets;
	
	UPROPERTY(EditDefaultsOnly, meta=(ForceInlineRow))
	TArray<FRarityStatRangeSet> RarityStatRangeSets;
};
