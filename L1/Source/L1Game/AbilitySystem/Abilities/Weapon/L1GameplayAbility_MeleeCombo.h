#pragma once

#include "L1GameplayAbility_MeleeAttack.h"
#include "L1GameplayAbility_MeleeCombo.generated.h"

UCLASS()
class UL1GameplayAbility_MeleeCombo : public UL1GameplayAbility_MeleeAttack
{
	GENERATED_BODY()

public:
	UL1GameplayAbility_MeleeCombo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void HandleMontageEvent(FGameplayEventData Payload) override;

private:
	void WaitInputContinue();
	void WaitInputStop();

private:
	UFUNCTION()
	void OnInputReleased(float TimeHeld);

	UFUNCTION()
	void OnInputStart();

	UFUNCTION()
	void OnInputCancel();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "L1|Melee Combo")
	TSubclassOf<UL1GameplayAbility_MeleeCombo> NextAbilityClass;

private:
	bool bInputPressed = false;
	bool bInputReleased = false;
};
