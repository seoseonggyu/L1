#pragma once

#include "L1PocketWorldAttachment.generated.h"

class UArrowComponent;

UCLASS(BlueprintType, Blueprintable)
class AL1PocketWorldAttachment : public AActor
{
	GENERATED_BODY()
	
public:
	AL1PocketWorldAttachment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UArrowComponent> ArrowComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> MeshComponent;
};
