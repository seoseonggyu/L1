#include "L1InventoryEntryWidget.h"

#include "L1InventorySlotsWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/SizeBox.h"
#include "Data/L1ItemData.h"
#include "Data/L1UIData.h"
#include "Item/L1ItemInstance.h"
#include "Item/Fragments/L1ItemFragment_Equipable.h"
#include "Item/Managers/L1InventoryManagerComponent.h"
#include "UI/Item/L1ItemDragDrop.h"
#include "UI/Item/L1ItemDragWidget.h"
#include "Player/LyraPlayerState.h"
#include "Character/LyraCharacter.h"
#include "Network/NetworkUtils.h"
#include "Network/L1NetworkManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1InventoryEntryWidget)

UL1InventoryEntryWidget::UL1InventoryEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UL1InventoryEntryWidget::Init(UL1InventorySlotsWidget* InSlotsWidget, UL1ItemInstance* InItemInstance, int32 InItemCount)
{
	if (InSlotsWidget == nullptr || InItemInstance == nullptr)
		return;
	
	SlotsWidget = InSlotsWidget;

	FIntPoint UnitInventorySlotSize = UL1UIData::Get().UnitInventorySlotSize;
	const UL1ItemTemplate& ItemTemplate = UL1ItemData::Get().FindItemTemplateByID(InItemInstance->GetItemTemplateID());

	FVector2D WidgetSize = FVector2D(ItemTemplate.SlotCount.X * UnitInventorySlotSize.X, ItemTemplate.SlotCount.Y * UnitInventorySlotSize.Y);
	SizeBox_Root->SetWidthOverride(WidgetSize.X);
	SizeBox_Root->SetHeightOverride(WidgetSize.Y);
	
	RefreshUI(InItemInstance, InItemCount);
}

FReply UL1InventoryEntryWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	FIntPoint UnitInventorySlotSize = UL1UIData::Get().UnitInventorySlotSize;
	
	FVector2D MouseWidgetPos = SlotsWidget->GetSlotContainerGeometry().AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	FVector2D ItemWidgetPos = SlotsWidget->GetSlotContainerGeometry().AbsoluteToLocal(InGeometry.LocalToAbsolute(UnitInventorySlotSize / 2.f));
	FIntPoint ItemSlotPos = FIntPoint(ItemWidgetPos.X / UnitInventorySlotSize.X, ItemWidgetPos.Y / UnitInventorySlotSize.Y);
	
	CachedFromSlotPos = ItemSlotPos;
	CachedDeltaWidgetPos = MouseWidgetPos - ItemWidgetPos;
	
	if (Reply.IsEventHandled() == false && UWidgetBlueprintLibrary::IsDragDropping() == false && InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		if (UL1InventoryManagerComponent* FromInventoryManager = SlotsWidget->GetInventoryManager())
		{
			if (UL1NetworkManager* NetworkManager = NetworkUtils::GetNetworkManager(Cast<ALyraPlayerState>(GetOwningPlayerState())))
			{
				NetworkManager->Check_QuickFromInventory(Cast<ALyraCharacter>(GetOwningPlayerPawn()), FromInventoryManager, ItemSlotPos);
				return FReply::Handled();
			}
		}
	}
	
	return Reply;
}

void UL1InventoryEntryWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	FIntPoint UnitInventorySlotSize = UL1UIData::Get().UnitInventorySlotSize;
	const UL1ItemTemplate& ItemTemplate = UL1ItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());

	TSubclassOf<UL1ItemDragWidget> DragWidgetClass = UL1UIData::Get().DragWidgetClass;
	UL1ItemDragWidget* DragWidget = CreateWidget<UL1ItemDragWidget>(GetOwningPlayer(), DragWidgetClass);
	FVector2D DragWidgetSize = FVector2D(ItemTemplate.SlotCount.X * UnitInventorySlotSize.X, ItemTemplate.SlotCount.Y * UnitInventorySlotSize.Y);
	DragWidget->Init(DragWidgetSize, ItemTemplate.IconTexture, ItemCount);
	
	UL1ItemDragDrop* DragDrop = NewObject<UL1ItemDragDrop>();
	DragDrop->DefaultDragVisual = DragWidget;
	DragDrop->Pivot = EDragPivot::TopLeft;
	DragDrop->Offset = -((CachedDeltaWidgetPos + UnitInventorySlotSize / 2.f) / DragWidgetSize);
	DragDrop->FromEntryWidget = this;
	DragDrop->FromInventoryManager = SlotsWidget->GetInventoryManager();
	DragDrop->FromItemSlotPos = CachedFromSlotPos;
	DragDrop->FromItemInstance = ItemInstance;
	DragDrop->DeltaWidgetPos = CachedDeltaWidgetPos;
	OutOperation = DragDrop;
}