#pragma once

#include "L1AnimNotify_SpawnActor.generated.h"

UCLASS()
class UL1AnimNotify_SpawnActor : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UL1AnimNotify_SpawnActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

public:
	UPROPERTY(EditAnywhere)
	FName SocketName;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ActorClass;
};
