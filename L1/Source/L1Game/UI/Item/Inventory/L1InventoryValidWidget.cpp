#include "L1InventoryValidWidget.h"

#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Data/L1UIData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1InventoryValidWidget)

UL1InventoryValidWidget::UL1InventoryValidWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UL1InventoryValidWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	FIntPoint UnitInventorySlotSize = UL1UIData::Get().UnitInventorySlotSize;
	SizeBox_Root->SetWidthOverride(UnitInventorySlotSize.X);
	SizeBox_Root->SetHeightOverride(UnitInventorySlotSize.Y);
}

void UL1InventoryValidWidget::ChangeSlotState(ESlotState SlotState)
{
	switch (SlotState)
	{
	case ESlotState::Default:
		Image_Red->SetVisibility(ESlateVisibility::Hidden);
		Image_Green->SetVisibility(ESlateVisibility::Hidden);
		break;
	case ESlotState::Invalid:
		Image_Red->SetVisibility(ESlateVisibility::Visible);
		Image_Green->SetVisibility(ESlateVisibility::Hidden);
		break;
	case ESlotState::Valid:
		Image_Red->SetVisibility(ESlateVisibility::Hidden);
		Image_Green->SetVisibility(ESlateVisibility::Visible);
		break;
	}
}
