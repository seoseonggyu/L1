#include "L1EquipmentSlotSingleWidget.h"

#include "L1EquipmentSlotsWidget.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Item/L1ItemInstance.h"
#include "Item/Fragments/L1ItemFragment_Equipable_Armor.h"
#include "Item/Fragments/L1ItemFragment_Equipable_Utility.h"
#include "Item/Managers/L1EquipManagerComponent.h"
#include "Item/Managers/L1EquipmentManagerComponent.h"
#include "Item/Managers/L1InventoryManagerComponent.h"
#include "System/LyraAssetManager.h"
#include "UI/Item/L1ItemDragDrop.h"
#include "UI/Item/Equipment/L1EquipmentEntryWidget.h"
#include "UI/Item/Inventory/L1InventoryEntryWidget.h"
#include "Data/L1UIData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1EquipmentSlotSingleWidget)

UL1EquipmentSlotSingleWidget::UL1EquipmentSlotSingleWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UL1EquipmentSlotSingleWidget::Init(EArmorType InArmorType, UL1EquipmentManagerComponent* InEquipmentManager)
{
	check(InArmorType != EArmorType::Count && InEquipmentManager != nullptr);

	EquipmentSingleSlotType = EEquipmentSingleSlotType::Armor;
	ArmorType = InArmorType;
	EquipmentManager = InEquipmentManager;
}

void UL1EquipmentSlotSingleWidget::Init(EUtilitySlotType InUtilitySlotType, UL1EquipmentManagerComponent* InEquipmentManager)
{
	check(InUtilitySlotType != EUtilitySlotType::Count && InEquipmentManager != nullptr);

	EquipmentSingleSlotType = EEquipmentSingleSlotType::Utility;
	UtilitySlotType = InUtilitySlotType;
	EquipmentManager = InEquipmentManager;
}

void UL1EquipmentSlotSingleWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (Image_BaseIcon)
	{
		Image_BaseIcon->SetBrushFromTexture(BaseIconTexture, true);
	}
}

bool UL1EquipmentSlotSingleWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
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
	EEquipmentSlotType ToEquipmentSlotType = GetEquipmentSlotType();
	if (ToEquipmentSlotType == EEquipmentSlotType::Count)
		return false;
	
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
	
	if (bIsValid)
	{
		Image_Red->SetVisibility(ESlateVisibility::Hidden);
		Image_Green->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Image_Red->SetVisibility(ESlateVisibility::Visible);
		Image_Green->SetVisibility(ESlateVisibility::Hidden);
	}
	return true;
}

bool UL1EquipmentSlotSingleWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
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

	// SSG: 
	/*UL1ItemManagerComponent* ItemManager = GetOwningPlayer()->FindComponentByClass<UL1ItemManagerComponent>();
	if (ItemManager == nullptr)
		return false;*/

	EEquipmentSlotType ToEquipmentSlotType = GetEquipmentSlotType();
	if (ToEquipmentSlotType == EEquipmentSlotType::Count)
		return false;

	const UL1ItemFragment* FromItemFragment = nullptr;
	switch (EquipmentSingleSlotType)
	{
	case EEquipmentSingleSlotType::Armor:	FromItemFragment = FromItemInstance->FindFragmentByClass<UL1ItemFragment_Equipable_Armor>();	break;
	case EEquipmentSingleSlotType::Utility:	FromItemFragment = FromItemInstance->FindFragmentByClass<UL1ItemFragment_Equipable_Utility>();	break;
	}
	if (FromItemFragment == nullptr)
		return false;
	
	// SSG: 
	/*if (UL1InventoryManagerComponent* FromInventoryManager = ItemDragDrop->FromInventoryManager)
	{
		ItemManager->Server_InventoryToEquipment(FromInventoryManager, ItemDragDrop->FromItemSlotPos, EquipmentManager, ToEquipmentSlotType);
	}
	else if (UL1EquipmentManagerComponent* FromEquipmentManager = ItemDragDrop->FromEquipmentManager)
	{
		ItemManager->Server_EquipmentToEquipment(FromEquipmentManager, ItemDragDrop->FromEquipmentSlotType, EquipmentManager, ToEquipmentSlotType);
	}*/
	
	return true;
}

void UL1EquipmentSlotSingleWidget::OnDragEnded()
{
	Super::OnDragEnded();

	Image_Red->SetVisibility(ESlateVisibility::Hidden);
	Image_Green->SetVisibility(ESlateVisibility::Hidden);
}

void UL1EquipmentSlotSingleWidget::OnEquipmentEntryChanged(UL1ItemInstance* InItemInstance, int32 InItemCount)
{
	if (EntryWidget)
	{
		Overlay_Entry->RemoveChild(EntryWidget);
		EntryWidget = nullptr;
	}
	
	if (InItemInstance)
	{
		TSubclassOf<UL1EquipmentEntryWidget> EntryWidgetClass = UL1UIData::Get().EquipmentEntryWidgetClass;
		EntryWidget = CreateWidget<UL1EquipmentEntryWidget>(GetOwningPlayer(), EntryWidgetClass);
		
		UOverlaySlot* OverlaySlot = Overlay_Entry->AddChildToOverlay(EntryWidget);
		OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
		OverlaySlot->SetVerticalAlignment(VAlign_Fill);
		
		EntryWidget->Init(InItemInstance, InItemCount, GetEquipmentSlotType(), EquipmentManager);

		Image_BaseIcon->SetRenderOpacity(0.f);
	}
	else
	{
		Image_BaseIcon->SetRenderOpacity(1.f);
	}
}

EEquipmentSlotType UL1EquipmentSlotSingleWidget::GetEquipmentSlotType() const
{
	switch (EquipmentSingleSlotType)
	{
	case EEquipmentSingleSlotType::Armor:	return UL1EquipManagerComponent::ConvertToEquipmentSlotType(ArmorType);
	case EEquipmentSingleSlotType::Utility:	return UL1EquipManagerComponent::ConvertToEquipmentSlotType(UtilitySlotType);
	}

	return EEquipmentSlotType::Count;
}
