#include "L1CheatMenuWidget.h"

#include "L1CheatEntryWidget.h"
#include "L1CheatListWidget.h"
#include "Components/Button.h"
#include "Data/L1CheatData.h"
#include "Data/L1ItemData.h"
#include "Item/L1ItemTemplate.h"
#include "Item/Fragments/L1ItemFragment_Equipable.h"
#include "Item/Fragments/L1ItemFragment_Equipable_Weapon.h"
#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1CheatMenuWidget)

UL1CheatMenuWidget::UL1CheatMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UL1CheatMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	CheatEntryWidgetClass = ULyraAssetManager::GetSubclassByName<UL1CheatEntryWidget>("CheatEntryWidgetClass");
	Button_Exit->OnClicked.AddUniqueDynamic(this, &ThisClass::OnExitButtonClicked);
}

void UL1CheatMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	const UL1ItemData& ItemData = ULyraAssetManager::Get().GetItemData();

	TArray<TSubclassOf<UL1ItemTemplate>> ItemTemplateClasses;
	ItemData.GetAllItemTemplateClasses(ItemTemplateClasses);

	for (TSubclassOf<UL1ItemTemplate> ItemTemplateClass : ItemTemplateClasses)
	{
		const UL1ItemTemplate* ItemTemplate = ItemTemplateClass.GetDefaultObject();
		if (const UL1ItemFragment_Equipable* EquippableFragment = ItemTemplate->FindFragmentByClass<UL1ItemFragment_Equipable>())
		{
			if (EquippableFragment->EquipmentType == EEquipmentType::Weapon)
			{
				const UL1ItemFragment_Equipable_Weapon* WeaponFragment = ItemTemplate->FindFragmentByClass<UL1ItemFragment_Equipable_Weapon>();
				if (WeaponFragment->WeaponType == EWeaponType::Unarmed)
					continue;
				
				UL1CheatEntryWidget* CheatEntryWidget = CreateWidget<UL1CheatEntryWidget>(GetOwningPlayer(), CheatEntryWidgetClass);
				CheatEntryWidget->InitializeUI(EL1CheatEntryType::PrimaryWeapon, ItemTemplateClass, nullptr);
				CheatList_PrimaryWeapon->AddEntry(CheatEntryWidget);

				CheatEntryWidget = CreateWidget<UL1CheatEntryWidget>(GetOwningPlayer(), CheatEntryWidgetClass);
				CheatEntryWidget->InitializeUI(EL1CheatEntryType::SecondaryWeapon, ItemTemplateClass, nullptr);
				CheatList_SecondaryWeapon->AddEntry(CheatEntryWidget);
			}
			else if (EquippableFragment->EquipmentType == EEquipmentType::Utility)
			{
				UL1CheatEntryWidget* CheatEntryWidget = CreateWidget<UL1CheatEntryWidget>(GetOwningPlayer(), CheatEntryWidgetClass);
				CheatEntryWidget->InitializeUI(EL1CheatEntryType::Utility, ItemTemplateClass, nullptr);
				CheatList_Utility->AddEntry(CheatEntryWidget);
			}
			else if (EquippableFragment->EquipmentType == EEquipmentType::Armor)
			{
				UL1CheatEntryWidget* CheatEntryWidget = CreateWidget<UL1CheatEntryWidget>(GetOwningPlayer(), CheatEntryWidgetClass);
				CheatEntryWidget->InitializeUI(EL1CheatEntryType::Armor, ItemTemplateClass, nullptr);
				CheatList_Armor->AddEntry(CheatEntryWidget);
			}
		}
	}

	const UL1CheatData& CheatData = ULyraAssetManager::Get().GetCheatData();
	
	for (const TSoftObjectPtr<UAnimMontage>& AnimMontagePath : CheatData.GetAnimMontagePaths())
	{
		UL1CheatEntryWidget* CheatEntryWidget = CreateWidget<UL1CheatEntryWidget>(GetOwningPlayer(), CheatEntryWidgetClass);
		CheatEntryWidget->InitializeUI(EL1CheatEntryType::Animation, nullptr, AnimMontagePath);
		CheatList_Animation->AddEntry(CheatEntryWidget);
	}
}

void UL1CheatMenuWidget::OnExitButtonClicked()
{
	DeactivateWidget();
}
