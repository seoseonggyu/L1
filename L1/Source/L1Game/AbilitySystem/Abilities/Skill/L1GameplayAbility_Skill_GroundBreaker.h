#pragma once

#include "AbilitySystem/Abilities/Weapon/L1GameplayAbility_Melee.h"
#include "L1GameplayAbility_Skill_GroundBreaker.generated.h"

UCLASS()
class UL1GameplayAbility_Skill_GroundBreaker : public UL1GameplayAbility_Melee
{
	GENERATED_BODY()
	
public:
	UL1GameplayAbility_Skill_GroundBreaker(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
protected:
	UFUNCTION()
	void OnGroundBreakerBegin(FGameplayEventData Payload);

	UFUNCTION()
	void OnMontageFinished();

private:
	void ExecuteGroundBreaker();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="L1|Ground Breaker")
	TObjectPtr<UAnimMontage> GroundBreakerMontage;
	
	UPROPERTY(EditDefaultsOnly, Category="L1|Ground Breaker")
	float DistanceOffset = 150.f;

	UPROPERTY(EditDefaultsOnly, Category="L1|Ground Breaker")
	float Damage = 80.f;
	
	UPROPERTY(EditDefaultsOnly, Category="L1|Ground Breaker")
	float StunDruation = 3.f;
};
