#include "L1PocketWorldAttachment.h"

#include "Components/ArrowComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1PocketWorldAttachment)

AL1PocketWorldAttachment::AL1PocketWorldAttachment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;
	
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("ArrowComponent");
	SetRootComponent(ArrowComponent);
	
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("MeshComponent");
	MeshComponent->SetCollisionProfileName("NoCollision");
	MeshComponent->SetGenerateOverlapEvents(false);
	MeshComponent->SetupAttachment(GetRootComponent());
}
