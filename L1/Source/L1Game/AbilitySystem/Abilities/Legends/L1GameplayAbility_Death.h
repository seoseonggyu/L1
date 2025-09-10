#pragma once

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "L1GameplayAbility_Death.generated.h"

struct FGameplayAbilityActorInfo;
struct FGameplayEventData;

UCLASS(Abstract)
class UL1GameplayAbility_Death : public ULyraGameplayAbility
{
	GENERATED_BODY()

public:
	UL1GameplayAbility_Death(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	void StartDeath();
	void FinishDeath();

protected:
	UPROPERTY(EditDefaultsOnly, Category="L1|Death")
	bool bAutoStartDeath = true;

	UPROPERTY(EditDefaultsOnly, Category="L1|Death")
	TSubclassOf<ULyraCameraMode> DeathCameraModeClass;

	UPROPERTY(EditDefaultsOnly, Category="L1|Death")
	float EndDelay = 1.5f;
};
