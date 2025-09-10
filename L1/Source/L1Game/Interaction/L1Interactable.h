#pragma once

#include "AbilitySystemBlueprintLibrary.h"
#include "L1InteractionInfo.h"
#include "L1InteractionQuery.h"
#include "AbilitySystem/Attributes/L1StatSet.h"
#include "UObject/Interface.h"
#include "L1Interactable.generated.h"

class IL1Interactable;
struct FL1InteractionInfo;
struct FL1InteractionQuery;

class FL1InteractionInfoBuilder
{
public:
	FL1InteractionInfoBuilder(TScriptInterface<IL1Interactable> InInteractable, TArray<FL1InteractionInfo>& InInteractionInfos)
		: Interactable(InInteractable)
		, InteractionInfos(InInteractionInfos) { }

public:
	void AddInteractionInfo(const FL1InteractionInfo& InteractionInfo)
	{
		FL1InteractionInfo& Entry = InteractionInfos.Add_GetRef(InteractionInfo);
		Entry.Interactable = Interactable;
	}
	
private:
	TScriptInterface<IL1Interactable> Interactable;
	TArray<FL1InteractionInfo>& InteractionInfos;
};

UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UL1Interactable : public UInterface
{
	GENERATED_BODY()
};

class IL1Interactable
{
	GENERATED_BODY()

public:
	virtual FL1InteractionInfo GetPreInteractionInfo(const FL1InteractionQuery& InteractionQuery) const { return FL1InteractionInfo(); }
	
	virtual void GatherPostInteractionInfos(const FL1InteractionQuery& InteractionQuery, FL1InteractionInfoBuilder& InteractionInfoBuilder) const
	{
		FL1InteractionInfo InteractionInfo = GetPreInteractionInfo(InteractionQuery);
		InteractionInfoBuilder.AddInteractionInfo(InteractionInfo);
	}
	
	virtual void CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag, FGameplayEventData& InOutEventData) const { }
	
	UFUNCTION(BlueprintCallable)
	virtual void GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const { }
	
	UFUNCTION(BlueprintCallable)
	virtual bool CanInteraction(const FL1InteractionQuery& InteractionQuery) const { return true; }
};
