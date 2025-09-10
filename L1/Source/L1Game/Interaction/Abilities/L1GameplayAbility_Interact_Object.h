#pragma once

#include "L1GameplayAbility_Interact_Info.h"
#include "L1GameplayAbility_Interact_Object.generated.h"

UCLASS()
class UL1GameplayAbility_Interact_Object : public UL1GameplayAbility_Interact_Info
{
	GENERATED_BODY()
	
public:
	UL1GameplayAbility_Interact_Object(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
protected:
	UFUNCTION()
	void OnInvalidInteraction();

protected:
	UPROPERTY(EditDefaultsOnly, Category="L1|Interaction")
	float AcceptanceAngle = 90.f;

	UPROPERTY(EditDefaultsOnly, Category="L1|Interaction")
	float AcceptanceDistance = 100.f;
	
protected:
	bool bInitialized = false;
};
