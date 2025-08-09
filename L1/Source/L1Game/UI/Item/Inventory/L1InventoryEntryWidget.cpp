#include "L1InventoryEntryWidget.h"

#include "L1InventorySlotsWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/SizeBox.h"
#include "Data/L1ItemData.h"
#include "Data/L1UIData.h"
#include "Item/L1ItemInstance.h"
#include "Item/Fragments/L1ItemFragment_Equipable.h"
#include "Item/Managers/L1InventoryManagerComponent.h"
#include "Item/Managers/L1EquipmentManagerComponent.h" // SSG: 
// #include "Item/Managers/L1ItemManagerComponent.h" // SSG: 
#include "UI/Item/L1ItemDragDrop.h"
#include "UI/Item/L1ItemDragWidget.h"

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
			QuickFromInventory(FromInventoryManager, ItemSlotPos);
			return FReply::Handled();
		}

		// SSG: 
		/*UL1ItemManagerComponent* ItemManager = GetOwningPlayer()->FindComponentByClass<UL1ItemManagerComponent>();

		if (ItemManager && FromInventoryManager)
		{
			ItemManager->Server_QuickFromInventory(FromInventoryManager, ItemSlotPos);
			return FReply::Handled();
		}*/
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

void UL1InventoryEntryWidget::QuickFromInventory(UL1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos)
{
	if (FromInventoryManager == nullptr)
		return;
	UL1InventoryManagerComponent* MyInventoryManager = GetMyInventoryManager();
	UL1EquipmentManagerComponent* MyEquipmentManager = GetMyEquipmentManager();
	if (MyInventoryManager == nullptr || MyEquipmentManager == nullptr)
		return;

	UL1ItemInstance* FromItemInstance = FromInventoryManager->GetItemInstance(FromItemSlotPos);
	if (FromItemInstance == nullptr)
		return;

	if (FromItemInstance->FindFragmentByClass<UL1ItemFragment_Equipable>())
	{
		// 1. [장비]
		// 1-1. [내 인벤토리] -> 내 장비 교체 -> 내 장비 장착 
		// 1-2. [다른 인벤토리] -> 내 장비 교체 -> 내 장비 장착 -> 내 인벤토리

		EEquipmentSlotType ToEquipmentSlotType;
		FIntPoint ToItemSlotPos;
		if (MyEquipmentManager->CanSwapEquipment_Quick(FromInventoryManager, FromItemSlotPos, ToEquipmentSlotType, ToItemSlotPos))
		{
			const int32 FromItemCount = FromInventoryManager->GetItemCount(FromItemSlotPos);
			const int32 ToItemCount = MyEquipmentManager->GetItemCount(ToEquipmentSlotType);

			UL1ItemInstance* RemovedItemInstanceFrom = FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, FromItemCount);
			UL1ItemInstance* RemovedItemInstanceTo = MyEquipmentManager->RemoveEquipment_Unsafe(ToEquipmentSlotType, ToItemCount);
			FromInventoryManager->AddItem_Unsafe(ToItemSlotPos, RemovedItemInstanceTo, ToItemCount);
			MyEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstanceFrom, FromItemCount);
		}
		else
		{
			int32 MovableCount = MyEquipmentManager->CanMoveOrMergeEquipment_Quick(FromInventoryManager, FromItemSlotPos, ToEquipmentSlotType);
			if (MovableCount > 0)
			{
				UL1ItemInstance* RemovedItemInstance = FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, MovableCount);
				MyEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstance, MovableCount);
			}
			else
			{
				if (MyInventoryManager != FromInventoryManager)
				{
					TArray<FIntPoint> OutToItemSlotPoses;
					TArray<int32> OutToItemCounts;
					MovableCount = MyInventoryManager->CanMoveOrMergeItem_Quick(FromInventoryManager, FromItemSlotPos, OutToItemSlotPoses, OutToItemCounts);
					if (MovableCount > 0)
					{
						UL1ItemInstance* RemovedItemInstance = FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, MovableCount);
						for (int32 i = 0; i < OutToItemSlotPoses.Num(); i++)
						{
							MyInventoryManager->AddItem_Unsafe(OutToItemSlotPoses[i], RemovedItemInstance, OutToItemCounts[i]);
						}
						return;
					}
				}
			}
		}
	}
	else
	{
		// 2. [일반 아이템]
		// 2-1. [내 인벤토리] -> X
		// 2-2. [다른 인벤토리] -> 내 인벤토리

		if (MyInventoryManager != FromInventoryManager)
		{
			TArray<FIntPoint> ToItemSlotPoses;
			TArray<int32> ToItemCounts;

			int32 MovableItemCount = MyInventoryManager->CanMoveOrMergeItem_Quick(FromInventoryManager, FromItemSlotPos, ToItemSlotPoses, ToItemCounts);
			if (MovableItemCount > 0)
			{
				UL1ItemInstance* RemovedItemInstance = FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, MovableItemCount);
				for (int32 i = 0; i < ToItemSlotPoses.Num(); i++)
				{
					MyInventoryManager->AddItem_Unsafe(ToItemSlotPoses[i], RemovedItemInstance, ToItemCounts[i]);
				}
			}
		}
	}
}

UL1InventoryManagerComponent* UL1InventoryEntryWidget::GetMyInventoryManager() const
{
	UL1InventoryManagerComponent* MyInventoryManager = nullptr;

	if (APlayerController* Controller = GetOwningPlayer())
	{
		if (APawn* Pawn = Controller->GetPawn())
		{
			MyInventoryManager = Pawn->GetComponentByClass<UL1InventoryManagerComponent>();
		}
	}

	return MyInventoryManager;
}

UL1EquipmentManagerComponent* UL1InventoryEntryWidget::GetMyEquipmentManager() const
{
	UL1EquipmentManagerComponent* MyEquipmentManager = nullptr;

	if (APlayerController* Controller = GetOwningPlayer())
	{
		if (APawn* Pawn = Controller->GetPawn())
		{
			MyEquipmentManager = Pawn->GetComponentByClass<UL1EquipmentManagerComponent>();

		}
	}
	return MyEquipmentManager;
}