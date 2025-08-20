#pragma once

#include "Blueprint/UserWidget.h"
#include "L1NumberPopWidget.generated.h"

class UTextBlock;

UCLASS()
class UL1NumberPopWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UL1NumberPopWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void InitializeUI(int32 InNumberToDisplay, FLinearColor InColor);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Number;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Animation_NumberPop;
};
