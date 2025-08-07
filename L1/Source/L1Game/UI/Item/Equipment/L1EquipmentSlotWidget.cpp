#include "L1EquipmentSlotWidget.h"

#include "L1EquipmentEntryWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1EquipmentSlotWidget)

UL1EquipmentSlotWidget::UL1EquipmentSlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UL1EquipmentSlotWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	OnDragEnded();
}

void UL1EquipmentSlotWidget::OnDragEnded()
{
	bAlreadyHovered = false;
}
