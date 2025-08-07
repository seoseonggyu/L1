#include "L1NumberPopWidgetDefault.h"

#include "L1LogChannels.h"
#include "Animation/UMGSequencePlayer.h"
#include "Components/TextBlock.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1NumberPopWidgetDefault)

UL1NumberPopWidgetDefault::UL1NumberPopWidgetDefault(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UL1NumberPopWidgetDefault::InitializeUI(int32 InNumberToDisplay, FLinearColor InColor)
{
	Super::InitializeUI(InNumberToDisplay, InColor);
	
	Text_Number->SetColorAndOpacity(InColor);
	Text_Number->SetText(FText::AsNumber(InNumberToDisplay));
	
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
	{
		PlayAnimation(Animation_NumberPop);
	});
}
