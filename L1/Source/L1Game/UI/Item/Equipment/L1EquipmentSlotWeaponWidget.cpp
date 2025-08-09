#include "L1EquipmentSlotWeaponWidget.h"

#include "CommonVisibilitySwitcher.h"
#include "L1Define.h"
#include "Item/L1ItemInstance.h"
#include "Item/Managers/L1EquipmentManagerComponent.h"
#include "L1EquipmentEntryWidget.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Data/L1UIData.h"
#include "Item/Fragments/L1ItemFragment_Equipable_Weapon.h"
#include "Item/Managers/L1EquipManagerComponent.h"
// #include "Item/Managers/L1ItemManagerComponent.h" // SSG: 
#include "Item/Managers/L1InventoryManagerComponent.h"
#include "UI/Item/L1ItemDragDrop.h"
#include "UI/Item/Inventory/L1InventoryEntryWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1EquipmentSlotWeaponWidget)

UL1EquipmentSlotWeaponWidget::UL1EquipmentSlotWeaponWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UL1EquipmentSlotWeaponWidget::Init(EWeaponSlotType InWeaponSlotType, UL1EquipmentManagerComponent* InEquipmentManager)
{
	check(InWeaponSlotType != EWeaponSlotType::Count && InEquipmentManager != nullptr);
	
	WeaponSlotType = InWeaponSlotType;
	EquipmentManager = InEquipmentManager;
}

void UL1EquipmentSlotWeaponWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	EntryWidgets.SetNum((int32)EWeaponHandType::Count);
	SlotRedImages = { Image_Red_LeftHand, Image_Red_RightHand, Image_Red_TwoHand };
	SlotGreenImages = { Image_Green_LeftHand, Image_Green_RightHand, Image_Green_TwoHand };
	SlotOverlays = { Overlay_LeftHandEntry, Overlay_RightHandEntry, Overlay_TwoHandEntry };
}

bool UL1EquipmentSlotWeaponWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);

	if (bAlreadyHovered)
		return true;
	
	bAlreadyHovered = true;
	
	UL1ItemDragDrop* ItemDragDrop = Cast<UL1ItemDragDrop>(InOperation);
	if (ItemDragDrop == nullptr)
		return false;
	
	UL1ItemInstance* FromItemInstance = ItemDragDrop->FromItemInstance;
	if (FromItemInstance == nullptr)
		return false;

	bool bIsValid = false;
	UImage* TargetImage = Image_Red_TwoHand;
	
	if (const UL1ItemFragment_Equipable_Weapon* FromWeaponFragment = FromItemInstance->FindFragmentByClass<UL1ItemFragment_Equipable_Weapon>())
	{
		EEquipmentSlotType ToEquipmentSlotType = UL1EquipManagerComponent::ConvertToEquipmentSlotType(FromWeaponFragment->WeaponHandType, WeaponSlotType);
	
		if (UL1InventoryManagerComponent* FromInventoryManager = ItemDragDrop->FromInventoryManager)
		{
			if (EquipmentManager->GetItemInstance(ToEquipmentSlotType))
			{
				FIntPoint OutToItemSlotPos;
				bIsValid = EquipmentManager->CanSwapEquipment(FromInventoryManager, ItemDragDrop->FromItemSlotPos, ToEquipmentSlotType, OutToItemSlotPos);
			}
			else
			{
				bIsValid = EquipmentManager->CanMoveOrMergeEquipment(FromInventoryManager, ItemDragDrop->FromItemSlotPos, ToEquipmentSlotType) > 0;
			}
		}
		else if (UL1EquipmentManagerComponent* FromEquipmentManager = ItemDragDrop->FromEquipmentManager)
		{
			if (EquipmentManager->GetItemInstance(ToEquipmentSlotType))
			{
				bIsValid = EquipmentManager->CanSwapEquipment(FromEquipmentManager, ItemDragDrop->FromEquipmentSlotType, ToEquipmentSlotType);
			}
			else
			{
				bIsValid = EquipmentManager->CanMoveOrMergeEquipment(FromEquipmentManager, ItemDragDrop->FromEquipmentSlotType, ToEquipmentSlotType) > 0;
			}
		}

		const int32 WeaponHandIndex = (int32)FromWeaponFragment->WeaponHandType;
		TargetImage = bIsValid ? SlotGreenImages[WeaponHandIndex] : SlotRedImages[WeaponHandIndex];
	}

	if (TargetImage)
	{
		TargetImage->SetVisibility(ESlateVisibility::Visible);
	}
	
	return true;
}

bool UL1EquipmentSlotWeaponWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	OnDragEnded();
	
	UL1ItemDragDrop* ItemDragDrop = Cast<UL1ItemDragDrop>(InOperation);
	if (ItemDragDrop == nullptr)
		return false;
	
	if (UL1ItemEntryWidget* FromEntryWidget = ItemDragDrop->FromEntryWidget)
	{
		FromEntryWidget->RefreshWidgetOpacity(true);
	}
	
	UL1ItemInstance* FromItemInstance = ItemDragDrop->FromItemInstance;
	if (FromItemInstance == nullptr)
		return false;

	
	// SSG: 아이템 옮기기 테스트 함수
	if (const UL1ItemFragment_Equipable_Weapon* FromWeaponFragment = FromItemInstance->FindFragmentByClass<UL1ItemFragment_Equipable_Weapon>())
	{
		EEquipmentSlotType ToEquipmentSlotType = UL1EquipManagerComponent::ConvertToEquipmentSlotType(FromWeaponFragment->WeaponHandType, WeaponSlotType);
	
		if (UL1InventoryManagerComponent* FromInventoryManager = ItemDragDrop->FromInventoryManager)
		{
			InventoryToEquipment(FromInventoryManager, ItemDragDrop->FromItemSlotPos, EquipmentManager, ToEquipmentSlotType);
		}
		else if (UL1EquipmentManagerComponent* FromEquipmentManager = ItemDragDrop->FromEquipmentManager)
		{
			EquipmentToEquipment(FromEquipmentManager, ItemDragDrop->FromEquipmentSlotType, EquipmentManager, ToEquipmentSlotType);
		}
	}
	return true;
}

void UL1EquipmentSlotWeaponWidget::OnDragEnded()
{
	Super::OnDragEnded();

	for (UImage* SlotGreenImage : SlotGreenImages)
	{
		SlotGreenImage->SetVisibility(ESlateVisibility::Hidden);
	}

	for (UImage* SlotRedImage : SlotRedImages)
	{
		SlotRedImage->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UL1EquipmentSlotWeaponWidget::OnEquipmentEntryChanged(EWeaponHandType InWeaponHandType, UL1ItemInstance* InItemInstance, int32 InItemCount)
{
	if (InWeaponHandType == EWeaponHandType::Count)
		return;

	const int32 WeaponHandIndex = (int32)InWeaponHandType;
	UOverlay* TargetOverlay = SlotOverlays[WeaponHandIndex];

	if (UL1EquipmentEntryWidget* EntryWidget = EntryWidgets[WeaponHandIndex])
	{
		if (UL1ItemInstance* ItemInstance = EntryWidget->GetItemInstance())
		{
			if (ItemInstance == InItemInstance)
			{
				EntryWidget->RefreshItemCount(InItemCount);
				return;
			}
		}
		
		TargetOverlay->RemoveChild(EntryWidget);
		EntryWidgets[WeaponHandIndex] = nullptr;
	}

	int32 ActiveWidgetIndex = Switcher_WeaponHandEntry->GetActiveWidgetIndex();
	
	if (InItemInstance)
	{
		TSubclassOf<UL1EquipmentEntryWidget> EntryWidgetClass = UL1UIData::Get().EquipmentEntryWidgetClass;
		UL1EquipmentEntryWidget* EntryWidget = CreateWidget<UL1EquipmentEntryWidget>(GetOwningPlayer(), EntryWidgetClass);
		EntryWidgets[WeaponHandIndex] = EntryWidget;
		
		UOverlaySlot* OverlaySlot = TargetOverlay->AddChildToOverlay(EntryWidget);
		OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
		OverlaySlot->SetVerticalAlignment(VAlign_Fill);
		
		EntryWidget->Init(InItemInstance, InItemCount, UL1EquipManagerComponent::ConvertToEquipmentSlotType(InWeaponHandType, WeaponSlotType), EquipmentManager);
		
		if (InWeaponHandType == EWeaponHandType::TwoHand)
			ActiveWidgetIndex = 1;
	}
	else
	{
		if (InWeaponHandType == EWeaponHandType::TwoHand)
			ActiveWidgetIndex = 0;
	}
	
	Switcher_WeaponHandEntry->SetActiveWidgetIndex(ActiveWidgetIndex);
}

void UL1EquipmentSlotWeaponWidget::InventoryToEquipment(UL1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, UL1EquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType)
{
	if (FromInventoryManager == nullptr || ToEquipmentManager == nullptr)
		return;

	int32 MovableCount = ToEquipmentManager->CanMoveOrMergeEquipment(FromInventoryManager, FromItemSlotPos, ToEquipmentSlotType);
	if (MovableCount > 0)
	{
		UL1ItemInstance* RemovedItemInstance = FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, MovableCount);
		ToEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstance, MovableCount);
	}
	else
	{
		FIntPoint ToItemSlotPos;
		if (ToEquipmentManager->CanSwapEquipment(FromInventoryManager, FromItemSlotPos, ToEquipmentSlotType, ToItemSlotPos))
		{
			const int32 FromItemCount = FromInventoryManager->GetItemCount(FromItemSlotPos);
			const int32 ToItemCount = ToEquipmentManager->GetItemCount(ToEquipmentSlotType);

			UL1ItemInstance* RemovedItemInstanceFrom = FromInventoryManager->RemoveItem_Unsafe(FromItemSlotPos, FromItemCount);
			UL1ItemInstance* RemovedItemInstanceTo = ToEquipmentManager->RemoveEquipment_Unsafe(ToEquipmentSlotType, ToItemCount);
			FromInventoryManager->AddItem_Unsafe(ToItemSlotPos, RemovedItemInstanceTo, ToItemCount);
			ToEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstanceFrom, FromItemCount);
		}
	}
}

void UL1EquipmentSlotWeaponWidget::EquipmentToEquipment(UL1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UL1EquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType)
{
	if (FromEquipmentManager == nullptr || ToEquipmentManager == nullptr)
		return;
	if (FromEquipmentManager == ToEquipmentManager && FromEquipmentSlotType == ToEquipmentSlotType)
		return;

	int32 MovableCount = ToEquipmentManager->CanMoveOrMergeEquipment(FromEquipmentManager, FromEquipmentSlotType, ToEquipmentSlotType);
	if (MovableCount > 0)
	{
		UL1ItemInstance* RemovedItemInstance = FromEquipmentManager->RemoveEquipment_Unsafe(FromEquipmentSlotType, MovableCount);
		ToEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstance, MovableCount);
	}
	else if (ToEquipmentManager->CanSwapEquipment(FromEquipmentManager, FromEquipmentSlotType, ToEquipmentSlotType))
	{
		const int32 FromItemCount = FromEquipmentManager->GetItemCount(FromEquipmentSlotType);
		const int32 ToItemCount = ToEquipmentManager->GetItemCount(ToEquipmentSlotType);

		UL1ItemInstance* RemovedItemInstanceFrom = FromEquipmentManager->RemoveEquipment_Unsafe(FromEquipmentSlotType, FromItemCount);
		UL1ItemInstance* RemovedItemInstanceTo = ToEquipmentManager->RemoveEquipment_Unsafe(ToEquipmentSlotType, ToItemCount);
		FromEquipmentManager->AddEquipment_Unsafe(FromEquipmentSlotType, RemovedItemInstanceTo, ToItemCount);
		ToEquipmentManager->AddEquipment_Unsafe(ToEquipmentSlotType, RemovedItemInstanceFrom, FromItemCount);
	}
}
