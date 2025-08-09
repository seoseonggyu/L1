#pragma once

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "L1GameplayAbility_ChangeEquip.generated.h"

UCLASS()
class UL1GameplayAbility_ChangeEquip : public ULyraGameplayAbility
{
	GENERATED_BODY()
	
public:
	UL1GameplayAbility_ChangeEquip(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="L1|Change Equip")
	float EndDelay = 0.6f;
};
