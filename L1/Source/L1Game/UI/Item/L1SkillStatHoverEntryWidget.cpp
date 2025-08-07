#include "L1SkillStatHoverEntryWidget.h"

#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1SkillStatHoverEntryWidget)

UL1SkillStatHoverEntryWidget::UL1SkillStatHoverEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UL1SkillStatHoverEntryWidget::RefreshUI(FText Name, FText Description)
{
	Text_Name->SetText(Name);
	Text_Description->SetText(Description);

	PlayAnimationForward(Animation_FadeIn);
}
