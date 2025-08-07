#pragma once

#include "Blueprint/UserWidget.h"
#include "L1InteractionWidget.generated.h"

UCLASS()
class UL1InteractionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UL1InteractionWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
