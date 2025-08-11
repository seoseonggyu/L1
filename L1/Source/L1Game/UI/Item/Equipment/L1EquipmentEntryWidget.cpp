#include "L1EquipmentEntryWidget.h"

#include "L1EquipmentSlotsWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Data/L1ItemData.h"
#include "Data/L1UIData.h"
#include "Item/L1ItemInstance.h"
#include "Item/L1ItemTemplate.h"
#include "UI/Item/L1ItemDragDrop.h"
#include "UI/Item/L1ItemDragWidget.h"
#include "Player/LyraPlayerState.h"
#include "Character/LyraCharacter.h"
#include "Network/NetworkUtils.h"
#include "Network/L1NetworkManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1EquipmentEntryWidget)

UL1EquipmentEntryWidget::UL1EquipmentEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UL1EquipmentEntryWidget::Init(UL1ItemInstance* InItemInstance, int32 InItemCount, EEquipmentSlotType InEquipmentSlotType, UL1EquipmentManagerComponent* InEquipmentManager)
{
	if (InEquipmentSlotType == EEquipmentSlotType::Unarmed_LeftHand || InEquipmentSlotType == EEquipmentSlotType::Unarmed_RightHand || InEquipmentSlotType == EEquipmentSlotType::Count)
		return;
	
	RefreshUI(InItemInstance, InItemCount);
	
	EquipmentSlotType = InEquipmentSlotType;
	EquipmentManager = InEquipmentManager;
}

void UL1EquipmentEntryWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	FIntPoint UnitInventorySlotSize = UL1UIData::Get().UnitInventorySlotSize;
	const UL1ItemTemplate& ItemTemplate = UL1ItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());

	TSubclassOf<UL1ItemDragWidget> DragWidgetClass = UL1UIData::Get().DragWidgetClass;
	UL1ItemDragWidget* ItemDragWidget = CreateWidget<UL1ItemDragWidget>(GetOwningPlayer(), DragWidgetClass);
	FVector2D DragWidgetSize = FVector2D(ItemTemplate.SlotCount * UnitInventorySlotSize);
	ItemDragWidget->Init(DragWidgetSize, ItemTemplate.IconTexture, ItemCount);
	
	UL1ItemDragDrop* ItemDragDrop = NewObject<UL1ItemDragDrop>();
	ItemDragDrop->DefaultDragVisual = ItemDragWidget;
	ItemDragDrop->Pivot = EDragPivot::CenterCenter;
	ItemDragDrop->FromEntryWidget = this;
	ItemDragDrop->FromEquipmentManager = EquipmentManager;
	ItemDragDrop->FromEquipmentSlotType = EquipmentSlotType;
	ItemDragDrop->FromItemInstance = ItemInstance;
	ItemDragDrop->DeltaWidgetPos = (DragWidgetSize / 2.f) - (UnitInventorySlotSize / 2.f);
	OutOperation = ItemDragDrop;
}

FReply UL1EquipmentEntryWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	
	if (Reply.IsEventHandled() == false && UWidgetBlueprintLibrary::IsDragDropping() == false && InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		if (UL1NetworkManager* NetworkManager = NetworkUtils::GetNetworkManager(Cast<ALyraPlayerState>(GetOwningPlayerState())))
		{
			NetworkManager->Check_QuickFromEquipment(Cast<ALyraCharacter>(GetOwningPlayerPawn()), EquipmentManager, EquipmentSlotType);
			return FReply::Handled();
		}
	}
	return Reply;
}