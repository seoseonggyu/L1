﻿#pragma once

#include "L1AnimNotify_PlayCameraShake.generated.h"

class ULegacyCameraShake;

UCLASS(meta=(DisplayName="Play Camera Shake"))
class UL1AnimNotify_PlayCameraShake : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UL1AnimNotify_PlayCameraShake(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<ULegacyCameraShake> CameraShakeClass;

	UPROPERTY(EditAnywhere)
	float PlayScale = 1.f;
};
