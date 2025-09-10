#pragma once

#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "Interaction/L1InteractionInfo.h"
#include "L1GameplayAbility_Interact.generated.h"

USTRUCT(BlueprintType)
struct FL1InteractionMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> Instigator = nullptr;

	UPROPERTY(BlueprintReadWrite)
	bool bShouldRefresh = false;

	UPROPERTY(BlueprintReadWrite)
	bool bSwitchActive = false;
	
	UPROPERTY(BlueprintReadWrite)
	FL1InteractionInfo InteractionInfo = FL1InteractionInfo();
};

UCLASS()
class UL1GameplayAbility_Interact : public ULyraGameplayAbility
{
	GENERATED_BODY()
	
public:
	UL1GameplayAbility_Interact(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION(BlueprintCallable)
	void UpdateInteractions(const TArray<FL1InteractionInfo>& InteractionInfos);

	UFUNCTION(BlueprintCallable)
	void TriggerInteraction();

private:
	void WaitInputStart();
	
	UFUNCTION()
	void OnInputStart();

protected:
	UPROPERTY(BlueprintReadWrite)
	TArray<FL1InteractionInfo> CurrentInteractionInfos;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="L1|Interaction")
	float InteractionTraceRange = 150.f;

	UPROPERTY(EditDefaultsOnly, Category="L1|Interaction")
	float InteractionTraceRate = 0.1f;
	
	UPROPERTY(EditDefaultsOnly, Category="L1|Interaction")
	float InteractionScanRange = 500.f;
	
	UPROPERTY(EditDefaultsOnly, Category="L1|Interaction")
	float InteractionScanRate = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category="L1|Interaction")
	bool bShowTraceDebug = false;
	
	UPROPERTY(EditDefaultsOnly, Category="L1|Interaction")
	TSoftClassPtr<UUserWidget> DefaultInteractionWidgetClass;
};
