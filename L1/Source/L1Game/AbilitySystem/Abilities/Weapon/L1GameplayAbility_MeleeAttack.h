#pragma once

#include "L1GameplayAbility_Melee.h"
#include "L1GameplayAbility_MeleeAttack.generated.h"

UCLASS()
class UL1GameplayAbility_MeleeAttack : public UL1GameplayAbility_Melee
{
	GENERATED_BODY()

public:
	UL1GameplayAbility_MeleeAttack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void HandleMontageEvent(FGameplayEventData Payload);

private:
	UFUNCTION()
	void OnTargetDataReady(FGameplayEventData Payload);

	UFUNCTION()
	void OnMontageEventTriggered(FGameplayEventData Payload);

	UFUNCTION()
	void OnMontageFinished();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "L1|Melee Attack")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "L1|Melee Attack")
	TObjectPtr<UAnimMontage> BackwardMontage;

protected:
	bool bBlocked = false;
	FTimerHandle BlockMontageTimerHandle;
};
