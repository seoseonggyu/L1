#include "L1NumberPopWidget.h"

#include "Components/TextBlock.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1NumberPopWidget)

UL1NumberPopWidget::UL1NumberPopWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UL1NumberPopWidget::InitializeUI(int32 InNumberToDisplay, FLinearColor InColor)
{
	Text_Number->SetColorAndOpacity(InColor);
	Text_Number->SetText(FText::AsNumber(InNumberToDisplay));
	
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
	{
		PlayAnimation(Animation_NumberPop);
	});
}
