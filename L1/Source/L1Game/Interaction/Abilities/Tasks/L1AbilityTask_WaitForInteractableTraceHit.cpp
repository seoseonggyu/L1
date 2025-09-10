#include "L1AbilityTask_WaitForInteractableTraceHit.h"

#include "AbilitySystemComponent.h"
#include "Interaction/L1Interactable.h"
#include "Interaction/L1InteractionInfo.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1AbilityTask_WaitForInteractableTraceHit)

UL1AbilityTask_WaitForInteractableTraceHit::UL1AbilityTask_WaitForInteractableTraceHit(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

UL1AbilityTask_WaitForInteractableTraceHit* UL1AbilityTask_WaitForInteractableTraceHit::WaitForInteractableTraceHit(UGameplayAbility* OwningAbility, FL1InteractionQuery InteractionQuery, ECollisionChannel TraceChannel, float InteractionTraceRate, bool bShowDebug)
{
	UL1AbilityTask_WaitForInteractableTraceHit* Task = NewAbilityTask<UL1AbilityTask_WaitForInteractableTraceHit>(OwningAbility);
	Task->InteractionTraceRate = InteractionTraceRate;
	Task->InteractionQuery = InteractionQuery;
	Task->TraceChannel = TraceChannel;
	Task->bShowDebug = bShowDebug;
	return Task;
}

void UL1AbilityTask_WaitForInteractableTraceHit::Activate()
{
	Super::Activate();
	
	SetWaitingOnAvatar();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(TraceTimerHandle, this, &ThisClass::PerformTrace, InteractionTraceRate, true);
	}
}

void UL1AbilityTask_WaitForInteractableTraceHit::OnDestroy(bool bInOwnerFinished)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TraceTimerHandle);
	}
	
	Super::OnDestroy(bInOwnerFinished);
}

void UL1AbilityTask_WaitForInteractableTraceHit::PerformTrace()
{
	AActor* AvatarActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	if (AvatarActor == nullptr)
		return;

	APlayerController* PlayerController = Ability->GetCurrentActorInfo()->PlayerController.Get();
	if (PlayerController == nullptr)
		return;

	FHitResult HitResult;
	if (PlayerController->GetHitResultUnderCursor(TraceChannel, true, HitResult) == false)
	{
		HighlightInteractables(CurrentInteractionInfos, false);
		CurrentInteractionInfos.Empty();
		return;
	}

	TArray<TScriptInterface<IL1Interactable>> Interactables;
	TScriptInterface<IL1Interactable> InteractableActor(HitResult.GetActor());
	if (InteractableActor)
	{
		Interactables.AddUnique(InteractableActor);
	}

	TScriptInterface<IL1Interactable> InteractableComponent(HitResult.GetComponent());
	if (InteractableComponent)
	{
		Interactables.AddUnique(InteractableComponent);
	}

	UpdateInteractionInfos(InteractionQuery, Interactables);
}

void UL1AbilityTask_WaitForInteractableTraceHit::HighlightInteractables(const TArray<FL1InteractionInfo>& InteractionInfos, bool bShouldHighlight)
{
	TArray<UMeshComponent*> MeshComponents;
	for (const FL1InteractionInfo& InteractionInfo : InteractionInfos)
	{
		if (IL1Interactable* Interactable = InteractionInfo.Interactable.GetInterface())
		{
			Interactable->GetMeshComponents(MeshComponents);
		}
	}

	for (UMeshComponent* MeshComponent : MeshComponents)
	{
		MeshComponent->SetRenderCustomDepth(bShouldHighlight);
	}
}

void UL1AbilityTask_WaitForInteractableTraceHit::UpdateInteractionInfos(const FL1InteractionQuery& InteractQuery, const TArray<TScriptInterface<IL1Interactable>>& Interactables)
{
	TArray<FL1InteractionInfo> NewInteractionInfos;

	for (const TScriptInterface<IL1Interactable>& Interactable : Interactables)
	{
		TArray<FL1InteractionInfo> TempInteractionInfos;
		FL1InteractionInfoBuilder InteractionInfoBuilder(Interactable, TempInteractionInfos);
		Interactable->GatherPostInteractionInfos(InteractQuery, InteractionInfoBuilder);
		for (FL1InteractionInfo& InteractionInfo : TempInteractionInfos)
		{
			NewInteractionInfos.Push(InteractionInfo);
		}
	}

	bool bInfosChanged = false;
	if (NewInteractionInfos.Num() == CurrentInteractionInfos.Num())
	{
		NewInteractionInfos.Sort();

		for (int InfoIndex = 0; InfoIndex < NewInteractionInfos.Num(); InfoIndex++)
		{
			const FL1InteractionInfo& NewInfo = NewInteractionInfos[InfoIndex];
			const FL1InteractionInfo& CurrentInfo = CurrentInteractionInfos[InfoIndex];

			if (NewInfo != CurrentInfo)
			{
				bInfosChanged = true;
				break;
			}
		}
	}
	else
	{
		bInfosChanged = true;
	}

	if (bInfosChanged)
	{
		HighlightInteractables(CurrentInteractionInfos, false);
		CurrentInteractionInfos = NewInteractionInfos;
		HighlightInteractables(CurrentInteractionInfos, true);

		InteractableChanged.Broadcast(CurrentInteractionInfos);
	}
}
