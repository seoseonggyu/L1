#include "L1SkillStatHoverWidget.h"

#include "L1SkillStatHoverEntryWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1SkillStatHoverWidget)

UL1SkillStatHoverWidget::UL1SkillStatHoverWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UL1SkillStatHoverWidget::RefreshUI(FText Name, FText Description)
{
	Widget_HoverEntry->RefreshUI(Name, Description);
}
