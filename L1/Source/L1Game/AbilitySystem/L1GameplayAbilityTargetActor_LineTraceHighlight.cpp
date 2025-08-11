#include "L1GameplayAbilityTargetActor_LineTraceHighlight.h"

#include "Character/LyraCharacter.h"
#include "Interaction/L1Interactable.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1GameplayAbilityTargetActor_LineTraceHighlight)

AL1GameplayAbilityTargetActor_LineTraceHighlight::AL1GameplayAbilityTargetActor_LineTraceHighlight(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void AL1GameplayAbilityTargetActor_LineTraceHighlight::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	HighlightActor(false, CachedTracedActor.Get());
	
	Super::EndPlay(EndPlayReason);
}

FHitResult AL1GameplayAbilityTargetActor_LineTraceHighlight::PerformTrace(AActor* InSourceActor)
{
	bool bTraceComplex = false;
	TArray<AActor*> ActorsToIgnore;

	ActorsToIgnore.Add(InSourceActor);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(L1GameplayAbilityTargetActor_LineTraceHightlight), bTraceComplex);
	Params.bReturnPhysicalMaterial = true;
	Params.AddIgnoredActors(ActorsToIgnore);

	FVector TraceStart = StartLocation.GetTargetingTransform().GetLocation();
	FVector TraceEnd;
	AimWithPlayerController(InSourceActor, Params, TraceStart, TraceEnd);

	FHitResult ReturnHitResult;
	LineTraceWithFilter(ReturnHitResult, InSourceActor->GetWorld(), Filter, TraceStart, TraceEnd, TraceProfile.Name, Params);
	
	if (CachedTracedActor.IsValid())
	{
		HighlightActor(false, CachedTracedActor.Get());
	}

	if (ReturnHitResult.bBlockingHit)
	{
		CachedTracedActor = ReturnHitResult.GetActor();
		HighlightActor(true, CachedTracedActor.Get());
	}
	else
	{
		ReturnHitResult.Location = TraceEnd;
	}

#if ENABLE_DRAW_DEBUG
	if (bDebug)
	{
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red);
	}
#endif // ENABLE_DRAW_DEBUG
	
	return ReturnHitResult;
}

void AL1GameplayAbilityTargetActor_LineTraceHighlight::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (AGameplayAbilityWorldReticle* LocalReticleActor = ReticleActor.Get())
	{
		if (ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(SourceActor))
		{
			LocalReticleActor->SetActorLocation(LyraCharacter->GetMesh()->GetComponentLocation());
		}
		else
		{
			LocalReticleActor->SetActorLocation(SourceActor ? SourceActor->GetActorLocation() : FVector::ZeroVector);
		}

		LocalReticleActor->SetActorRotation(FRotator::ZeroRotator);
	}
}

void AL1GameplayAbilityTargetActor_LineTraceHighlight::HighlightActor(bool bShouldHighlight, AActor* ActorToHighlight)
{
	if (IsValid(ActorToHighlight) == false)
		return;

	TArray<UMeshComponent*> MeshComponents;
	
	if (IL1Interactable* Interactable = Cast<IL1Interactable>(ActorToHighlight))
	{
		Interactable->GetMeshComponents(MeshComponents);
	}
	else
	{
		ActorToHighlight->GetComponents<UMeshComponent>(MeshComponents);
	}
	
	for (UMeshComponent* MeshComponent : MeshComponents)
	{
		MeshComponent->SetRenderCustomDepth(bShouldHighlight);
	}
}
