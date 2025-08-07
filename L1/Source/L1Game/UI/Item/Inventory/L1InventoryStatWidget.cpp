#include "L1InventoryStatWidget.h"

#include "Data/L1UIData.h"
#include "UI/Item/L1SkillStatHoverWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1InventoryStatWidget)

UL1InventoryStatWidget::UL1InventoryStatWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UL1InventoryStatWidget::NativeDestruct()
{
	if (HoverWidget)
	{
		HoverWidget->RemoveFromParent();
		HoverWidget = nullptr;
	}
	
	Super::NativeDestruct();
}

void UL1InventoryStatWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	const FL1UIInfo& UIInfo = UL1UIData::Get().GetTagUIInfo(StatTag);
	if (UIInfo.Title.IsEmpty() == false && UIInfo.Content.IsEmpty() == false)
	{
		if (HoverWidget == nullptr)
		{
			TSubclassOf<UL1SkillStatHoverWidget> HoverWidgetClass = UL1UIData::Get().SkillStatHoverWidget;
			HoverWidget = CreateWidget<UL1SkillStatHoverWidget>(GetOwningPlayer(), HoverWidgetClass);
		}

		if (HoverWidget)
		{
		
			HoverWidget->RefreshUI(UIInfo.Title, UIInfo.Content);
			HoverWidget->AddToViewport();
		}
	}
}

FReply UL1InventoryStatWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseMove(InGeometry, InMouseEvent);

	if (HoverWidget)
	{
		HoverWidget->SetPosition(InMouseEvent.GetScreenSpacePosition());
		return FReply::Handled();
	}
	
	return Reply;
}

void UL1InventoryStatWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	if (HoverWidget)
	{
		HoverWidget->RemoveFromParent();
		HoverWidget = nullptr;
	}
}
