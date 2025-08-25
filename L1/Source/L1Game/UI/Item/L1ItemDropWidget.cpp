#include "L1ItemDropWidget.h"

#include "L1ItemDragDrop.h"
#include "L1ItemEntryWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1ItemDropWidget)

UL1ItemDropWidget::UL1ItemDropWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

bool UL1ItemDropWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	if (UL1ItemDragDrop* ItemDragDrop = Cast<UL1ItemDragDrop>(InOperation))
	{
		if (UL1ItemEntryWidget* FromEntryWidget = ItemDragDrop->FromEntryWidget)
		{
			FromEntryWidget->RefreshWidgetOpacity(true);
		}

		// SSG: 
		/*if (UL1ItemManagerComponent* ItemManager = GetOwningPlayer()->FindComponentByClass<UL1ItemManagerComponent>())
		{
			if (ItemDragDrop->FromInventoryManager)
			{
				ItemManager->Server_DropItemFromInventory(ItemDragDrop->FromInventoryManager, ItemDragDrop->FromItemSlotPos);
			}
			else if (ItemDragDrop->FromEquipmentManager)
			{
				ItemManager->Server_DropItemFromEquipment(ItemDragDrop->FromEquipmentManager, ItemDragDrop->FromEquipmentSlotType);
			}
		}*/
	}

	return true;
}
