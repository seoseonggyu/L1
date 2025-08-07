#pragma once

#include "L1NumberPopWidgetBase.h"
#include "L1NumberPopWidgetDefault.generated.h"

class UTextBlock;

UCLASS()
class UL1NumberPopWidgetDefault : public UL1NumberPopWidgetBase
{
	GENERATED_BODY()
	
public:
	UL1NumberPopWidgetDefault(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void InitializeUI(int32 InNumberToDisplay, FLinearColor InColor) override;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Number;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Animation_NumberPop;
};
