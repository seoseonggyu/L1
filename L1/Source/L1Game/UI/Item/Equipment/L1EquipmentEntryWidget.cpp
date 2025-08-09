#include "L1EquipmentEntryWidget.h"

#include "L1EquipmentSlotsWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Data/L1ItemData.h"
#include "Data/L1UIData.h"
#include "Item/L1ItemInstance.h"
#include "Item/L1ItemTemplate.h"
#include "Item/Managers/L1InventoryManagerComponent.h"	 // SSG: 아이템 임시 옮기기
#include "Item/Managers/L1EquipmentManagerComponent.h"	 // SSG: 아이템 임시 옮기기
#include "UI/Item/L1ItemDragDrop.h"
#include "UI/Item/L1ItemDragWidget.h"
// #include "Item/Managers/L1ItemManagerComponent.h" // SSG: 

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
		{
			QuickFromEquipment(EquipmentManager, EquipmentSlotType);
			return FReply::Handled();
		}
	}
	return Reply;
}

void UL1EquipmentEntryWidget::QuickFromEquipment(UL1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType)
{
	if (FromEquipmentManager == nullptr || FromEquipmentSlotType == EEquipmentSlotType::Count)
		return;

	// 1. [내 장비창] -> 내 인벤토리
	// 2. [다른 장비창] -> 내 장비 교체 -> 내 장비 장착 -> 내 인벤토리 

	UL1InventoryManagerComponent* MyInventoryManager = GetMyInventoryManager();
	UL1EquipmentManagerComponent* MyEquipmentManager = GetMyEquipmentManager();
	if (MyInventoryManager == nullptr || MyEquipmentManager == nullptr)
		return;

	if (MyEquipmentManager == FromEquipmentManager)
	{
		TArray<FIntPoint> ToItemSlotPoses;
		TArray<int32> ToItemCounts;

		int32 MovableCount = MyInventoryManager->CanMoveOrMergeItem_Quick(FromEquipmentManager, FromEquipmentSlotType, ToItemSlotPoses, ToItemCounts);
		if (MovableCount > 0)
		{
			UL1ItemInstance* RemovedItemInstance = FromEquipmentManager->RemoveEquipment_Unsafe(FromEquipmentSlotType, MovableCount);
			for (int32 i = 0; i < ToItemSlotPoses.Num(); i++)
			{
				MyInventoryManager->AddItem_Unsafe(ToItemSlotPoses[i], RemovedItemInstance, ToItemCounts[i]);
			}
		}
	}
	else
	{
		EEquipmentSlotType ToEquipmentSlotType;
		if (MyEquipmentManager->CanSwapEquipment_Quick(FromEquipmentManager, FromEquipmentSlotType, ToEquipmentSlotType))
		{
			const int32 FromItemCount = FromEquipmentManager->GetItemCount(FromEquipmentSlotType);
			const int32 ToItemCount = MyEquipmentManager->GetItemCount(ToEquipmentSlotType);

			UL1ItemInstance* RemovedItemInstanceFrom = FromEquipmentManager->RemoveEquipment_Unsafe(FromEquipmentSlotType, FromItemCount);
			UL1ItemInstance* RemovedItemInstanceTo = MyEquipmentManager->RemoveEquipment_Unsafe(ToEquipmentSlotType, ToItemCount);
			FromEquipmentManager->AddEquipment_Unsafe(FromEquipmentSlotType, RemovedItemInstanceTo, ToItemCount);
			MyEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstanceFrom, FromItemCount);
		}
		else
		{
			int32 MovableCount = MyEquipmentManager->CanMoveOrMergeEquipment_Quick(FromEquipmentManager, FromEquipmentSlotType, ToEquipmentSlotType);
			if (MovableCount > 0)
			{
				UL1ItemInstance* RemovedItemInstance = FromEquipmentManager->RemoveEquipment_Unsafe(FromEquipmentSlotType, MovableCount);
				MyEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstance, MovableCount);
			}
			else
			{
				TArray<FIntPoint> ToItemSlotPoses;
				TArray<int32> ToItemCounts;

				MovableCount = MyInventoryManager->CanMoveOrMergeItem_Quick(FromEquipmentManager, FromEquipmentSlotType, ToItemSlotPoses, ToItemCounts);
				if (MovableCount > 0)
				{
					UL1ItemInstance* RemovedItemInstance = FromEquipmentManager->RemoveEquipment_Unsafe(FromEquipmentSlotType, MovableCount);
					for (int32 i = 0; i < ToItemSlotPoses.Num(); i++)
					{
						MyInventoryManager->AddItem_Unsafe(ToItemSlotPoses[i], RemovedItemInstance, ToItemCounts[i]);
					}
				}
			}
		}
	}
}

UL1InventoryManagerComponent* UL1EquipmentEntryWidget::GetMyInventoryManager() const
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

UL1EquipmentManagerComponent* UL1EquipmentEntryWidget::GetMyEquipmentManager() const
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
