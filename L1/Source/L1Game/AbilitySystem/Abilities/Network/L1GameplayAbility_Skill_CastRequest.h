#pragma once

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "L1GameplayAbility_Skill_CastRequest.generated.h"


UCLASS(Blueprintable)
class UL1GameplayAbility_Skill_CastRequest : public ULyraGameplayAbility
{
	GENERATED_BODY()

public:
	UL1GameplayAbility_Skill_CastRequest(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "L1|Network", meta = (Categories = "Network"))
	FGameplayTag SendSkillTag;
};