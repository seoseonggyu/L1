#pragma once

#include "L1GameplayAbility_Interact_Info.h"
#include "L1GameplayAbility_Interact_Active.generated.h"

UCLASS()
class UL1GameplayAbility_Interact_Active : public UL1GameplayAbility_Interact_Info
{
	GENERATED_BODY()
	
public:
	UL1GameplayAbility_Interact_Active(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UFUNCTION()
	void OnInvalidInteraction();

	UFUNCTION()
	void OnInputReleased(float TimeHeld);
	
	UFUNCTION()
	void OnDurationEnded();

	UFUNCTION()
	void OnNetSync();
	
	UFUNCTION()
	bool TriggerInteraction();

protected:
	UPROPERTY(EditDefaultsOnly, Category="L1|Interaction")
	TObjectPtr<UInputAction> MoveInputAction;
	
	UPROPERTY(EditDefaultsOnly, Category="L1|Interaction")
	float AcceptanceAngle = 65.f;

	UPROPERTY(EditDefaultsOnly, Category="L1|Interaction")
	float AcceptanceDistance = 10.f;
};
