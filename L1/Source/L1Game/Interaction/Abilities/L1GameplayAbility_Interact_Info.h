#pragma once

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "Interaction/L1InteractionInfo.h"
#include "L1GameplayAbility_Interact_Info.generated.h"

UCLASS()
class UL1GameplayAbility_Interact_Info : public ULyraGameplayAbility
{
	GENERATED_BODY()
	
public:
	UL1GameplayAbility_Interact_Info(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UFUNCTION(BlueprintCallable)
	bool InitializeAbility(AActor* TargetActor);

protected:
	UPROPERTY(BlueprintReadOnly)
	TScriptInterface<IL1Interactable> Interactable;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> InteractableActor;
	
	UPROPERTY(BlueprintReadOnly)
	FL1InteractionInfo InteractionInfo;
};
