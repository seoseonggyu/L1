#pragma once

#include "Blueprint/UserWidget.h"
#include "L1NumberPopWidgetBase.generated.h"

UCLASS()
class UL1NumberPopWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UL1NumberPopWidgetBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void InitializeUI(int32 InNumberToDisplay, FLinearColor InColor);
};
