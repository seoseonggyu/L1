#include "L1ItemHoverWidget.h"

#include "L1ItemHoverEntryWidget.h"
#include "Item/L1ItemInstance.h"
#include "Item/Managers/L1EquipmentManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1ItemHoverWidget)

UL1ItemHoverWidget::UL1ItemHoverWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UL1ItemHoverWidget::RefreshUI(const UL1ItemInstance* HoveredItemInstance)
{
	const UL1ItemInstance* PairItemInstance = nullptr;
	if (UL1EquipmentManagerComponent* EquipmentManager = GetOwningPlayerPawn()->GetComponentByClass<UL1EquipmentManagerComponent>())
	{
		EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;
		PairItemInstance = EquipmentManager->FindPairItemInstance(HoveredItemInstance, EquipmentSlotType);
	}
	
	HoverWidget_Left->RefreshUI(HoveredItemInstance);
	HoverWidget_Right->RefreshUI(PairItemInstance);
}
