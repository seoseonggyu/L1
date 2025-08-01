// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/LyraCameraMode.h"
#include "L1CameraMode_TopDown.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class L1GAME_API UL1CameraMode_TopDown : public ULyraCameraMode
{
	GENERATED_BODY()
	
public:
	UL1CameraMode_TopDown();

protected:

	//~ULyraCameraMode interface
	virtual void UpdateView(float DeltaTime) override;
	//~End of ULyraCameraMode interface


};
