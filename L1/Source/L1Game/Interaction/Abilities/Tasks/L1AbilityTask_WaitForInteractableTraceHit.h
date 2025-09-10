#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "Interaction/L1InteractionQuery.h"
#include "L1AbilityTask_WaitForInteractableTraceHit.generated.h"

struct FL1InteractionInfo;
class IL1Interactable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractableChanged, const TArray<FL1InteractionInfo>&, InteractableInfos);

UCLASS()
class UL1AbilityTask_WaitForInteractableTraceHit : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UL1AbilityTask_WaitForInteractableTraceHit(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="true"))
	static UL1AbilityTask_WaitForInteractableTraceHit* WaitForInteractableTraceHit(UGameplayAbility* OwningAbility, FL1InteractionQuery InteractionQuery, ECollisionChannel TraceChannel, float InteractionTraceRate = 0.1f, bool bShowDebug = false);

protected:
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:
	void PerformTrace();
	
	void HighlightInteractables(const TArray<FL1InteractionInfo>& InteractionInfos, bool bShouldHighlight);
	void UpdateInteractionInfos(const FL1InteractionQuery& InteractQuery, const TArray<TScriptInterface<IL1Interactable>>& Interactables);


public:
	UPROPERTY(BlueprintAssignable)
	FOnInteractableChanged InteractableChanged;

private:
	UPROPERTY()
	FL1InteractionQuery InteractionQuery;

	ECollisionChannel TraceChannel = ECC_Visibility;
	float InteractionTraceRate = 0.1f;
	bool bShowDebug = false;
	
	FTimerHandle TraceTimerHandle;
	TArray<FL1InteractionInfo> CurrentInteractionInfos;

};
