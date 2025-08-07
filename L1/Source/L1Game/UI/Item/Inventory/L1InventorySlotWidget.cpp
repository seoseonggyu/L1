#include "L1InventorySlotWidget.h"

#include "Components/SizeBox.h"
#include "Data/L1UIData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1InventorySlotWidget)

UL1InventorySlotWidget::UL1InventorySlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UL1InventorySlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	FIntPoint UnitInventorySlotSize = UL1UIData::Get().UnitInventorySlotSize;
	SizeBox_Root->SetWidthOverride(UnitInventorySlotSize.X);
	SizeBox_Root->SetHeightOverride(UnitInventorySlotSize.Y);
}
