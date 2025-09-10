#pragma once

#include "L1GameplayAbility_Interact_Object.h"
#include "L1GameplayAbility_Interact_Pickup.generated.h"

UCLASS()
class UL1GameplayAbility_Interact_Pickup : public UL1GameplayAbility_Interact_Object
{
	GENERATED_BODY()
	
public:
	UL1GameplayAbility_Interact_Pickup(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
