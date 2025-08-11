#pragma once

#include "L1Define.h"
#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "Character/LyraCharacter.h"
#include "L1GameplayAbility_Equipment.generated.h"

class UL1ItemInstance;
class AL1EquipmentBase;

USTRUCT(BlueprintType)
struct FL1EquipmentInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="L1|Equipment")
	EEquipmentType EquipmentType = EEquipmentType::Count;

public:
	UPROPERTY(EditAnywhere, Category="L1|Equipment", meta=(EditCondition="EquipmentType==EEquipmentType::Armor", EditConditionHides))
	EArmorType RequiredArmorType = EArmorType::Count;

public:
	UPROPERTY(EditAnywhere, Category="L1|Equipment", meta=(EditCondition="EquipmentType==EEquipmentType::Weapon", EditConditionHides))
	EWeaponHandType WeaponHandType = EWeaponHandType::Count;
	
	UPROPERTY(EditAnywhere, Category="L1|Equipment", meta=(EditCondition="EquipmentType==EEquipmentType::Weapon", EditConditionHides))
	EWeaponType RequiredWeaponType = EWeaponType::Count;

public:
	UPROPERTY(EditAnywhere, Category="L1|Equipment", meta=(EditCondition="EquipmentType==EEquipmentType::Utility", EditConditionHides))
	EUtilityType RequiredUtilityType = EUtilityType::Count;

public:
	UPROPERTY()
	TWeakObjectPtr<AL1EquipmentBase> EquipmentActor;
};

UCLASS(Blueprintable)
class UL1GameplayAbility_Equipment : public ULyraGameplayAbility
{
	GENERATED_BODY()
	
public:
	UL1GameplayAbility_Equipment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

public:
	AL1EquipmentBase* GetFirstEquipmentActor() const;
	UL1ItemInstance* GetEquipmentItemInstance(const AL1EquipmentBase* InEquipmentActor) const;
	
	int32 GetEquipmentStatValue(FGameplayTag InStatTag, const AL1EquipmentBase* InEquipmentActor) const;
	float GetSnapshottedAttackRate() const { return SnapshottedAttackRate; }
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="L1|Equipment")
	TArray<FL1EquipmentInfo> EquipmentInfos;
	
	UPROPERTY(EditDefaultsOnly, Category="L1|Equipment")
	float DefaultAttackRate = 1.f;

	UPROPERTY(EditDefaultsOnly, Category="L1|Equipment")
	TSubclassOf<ULyraCameraMode> CameraModeClass;

private:
	float SnapshottedAttackRate = 0.f;
};
