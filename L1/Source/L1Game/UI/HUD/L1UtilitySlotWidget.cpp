#include "L1UtilitySlotWidget.h"

#include "CommonVisibilitySwitcher.h"
#include "Character/LyraCharacter.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/L1ItemData.h"
#include "Item/L1ItemInstance.h"
#include "Item/L1ItemTemplate.h"
#include "Item/Managers/L1EquipManagerComponent.h"
#include "Item/Managers/L1EquipmentManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1UtilitySlotWidget)

UL1UtilitySlotWidget::UL1UtilitySlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UL1UtilitySlotWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	FText SlotNumber = FText::AsNumber((int32)EWeaponSlotType::Count + (int32)WidgetUtilitySlotType + 1);
	Text_SlotNumber->SetText(SlotNumber);
}

void UL1UtilitySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(GetOwningPlayerPawn());
	if (LyraCharacter == nullptr)
		return;
	
	EquipManager = LyraCharacter->GetComponentByClass<UL1EquipManagerComponent>();
	EquipmentManager = LyraCharacter->GetComponentByClass<UL1EquipmentManagerComponent>();
	if (EquipManager == nullptr || EquipmentManager == nullptr)
		return;

	const TArray<FL1EquipmentEntry>& Entries = EquipmentManager->GetEntries();
	for (int32 i = 0; i < Entries.Num(); i++)
	{
		const FL1EquipmentEntry& Entry = Entries[i];
		if (UL1ItemInstance* ItemInstance = Entry.GetItemInstance())
		{
			OnEquipmentEntryChanged((EEquipmentSlotType)i, ItemInstance, Entry.GetItemCount());
		}
	}
	EntryChangedDelegateHandle = EquipmentManager->OnEquipmentEntryChanged.AddUObject(this, &ThisClass::OnEquipmentEntryChanged);
	
	EEquipState CurrentEquipState = EquipManager->GetCurrentEquipState();
	OnEquipStateChanged(CurrentEquipState, CurrentEquipState);
	EquipStateChangedDelegateHandle = EquipManager->OnEquipStateChanged.AddUObject(this, &ThisClass::OnEquipStateChanged);
}

void UL1UtilitySlotWidget::NativeDestruct()
{
	if (EquipmentManager)
	{
		EquipmentManager->OnEquipmentEntryChanged.Remove(EntryChangedDelegateHandle);
		EntryChangedDelegateHandle.Reset();
	}

	if (EquipManager)
	{
		EquipManager->OnEquipStateChanged.Remove(EquipStateChangedDelegateHandle);
		EquipStateChangedDelegateHandle.Reset();
	}
	
	Super::NativeDestruct();
}

void UL1UtilitySlotWidget::OnEquipmentEntryChanged(EEquipmentSlotType EquipmentSlotType, UL1ItemInstance* ItemInstance, int32 ItemCount)
{
	EUtilitySlotType EntryUtilitySlotType = UL1EquipManagerComponent::ConvertToUtilitySlotType(EquipmentSlotType);
	if (EntryUtilitySlotType != WidgetUtilitySlotType)
		return;
	
	if (ItemInstance)
	{
		const UL1ItemTemplate& ItemTemplate = UL1ItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());

		Image_Icon->SetBrushFromTexture(ItemTemplate.IconTexture, true);
		Image_Icon->SetVisibility(ESlateVisibility::HitTestInvisible);

		if (ItemCount > 1)
		{
			Text_Count->SetText(FText::AsNumber(ItemCount));
			Text_Count->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			Text_Count->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else
	{
		Image_Icon->SetVisibility(ESlateVisibility::Hidden);
		Text_Count->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UL1UtilitySlotWidget::OnEquipStateChanged(EEquipState PrevEquipState, EEquipState NewEquipState)
{
	EEquipState SlotEquipState = UL1EquipManagerComponent::ConvertToEquipState(WidgetUtilitySlotType);

	if (NewEquipState == SlotEquipState)
	{
		PlayAnimationForward(Animation_Highlight_In);
	}
	else if (PrevEquipState == SlotEquipState)
	{
		PlayAnimationReverse(Animation_Highlight_In);
	}
}
