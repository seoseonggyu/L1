#pragma once

#include "EnhancedPlayerInput.h"
#include "L1EnhancedPlayerInput.generated.h"

UCLASS()
class UL1EnhancedPlayerInput : public UEnhancedPlayerInput
{
	GENERATED_BODY()
	
public:
	UL1EnhancedPlayerInput();

public:
	void FlushPressedInput(UInputAction* InputAction);
};
