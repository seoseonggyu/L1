#include "L1GameplayAbility_Interact_Info.h"

#include "Interaction/L1Interactable.h"
#include "Interaction/L1InteractionQuery.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1GameplayAbility_Interact_Info)

UL1GameplayAbility_Interact_Info::UL1GameplayAbility_Interact_Info(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

bool UL1GameplayAbility_Interact_Info::InitializeAbility(AActor* TargetActor)
{
	TScriptInterface<IL1Interactable> TargetInteractable(TargetActor);
	if (TargetInteractable)
	{
		FL1InteractionQuery InteractionQuery;
		InteractionQuery.RequestingAvatar = GetAvatarActorFromActorInfo();
		InteractionQuery.RequestingController = GetControllerFromActorInfo();

		Interactable = TargetInteractable;
		InteractableActor = TargetActor;

		TArray<FL1InteractionInfo> InteractionInfos;
		FL1InteractionInfoBuilder InteractionInfoBuilder(Interactable, InteractionInfos);
		Interactable->GatherPostInteractionInfos(InteractionQuery, InteractionInfoBuilder);
		InteractionInfo = InteractionInfos[0];

		return true;
	}

	return false;
}
