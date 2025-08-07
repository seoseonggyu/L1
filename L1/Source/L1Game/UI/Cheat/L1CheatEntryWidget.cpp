#include "L1CheatEntryWidget.h"

#include "CommonTextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Item/L1ItemTemplate.h"
#include "Player/LyraCheatManager.h"
#include "Player/LyraPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1CheatEntryWidget)

UL1CheatEntryWidget::UL1CheatEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UL1CheatEntryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_Entry->OnClicked.AddUniqueDynamic(this, &ThisClass::OnButtonClicked);
}

void UL1CheatEntryWidget::InitializeUI(EL1CheatEntryType InCheatEntryType, TSubclassOf<UL1ItemTemplate> InItemTemplateClass, TSoftObjectPtr<UAnimMontage> InAnimMontage)
{
	CheatEntryType = InCheatEntryType;
	
	switch (CheatEntryType)
	{
	case EL1CheatEntryType::PrimaryWeapon:
	case EL1CheatEntryType::SecondaryWeapon:
	case EL1CheatEntryType::Armor:
	case EL1CheatEntryType::Utility:
		ItemTemplateClass = InItemTemplateClass;
		if (ItemTemplateClass)
		{
			if (UL1ItemTemplate* ItemTemplate = Cast<UL1ItemTemplate>(ItemTemplateClass->GetDefaultObject()))
			{
				Text_Entry->SetText(ItemTemplate->DisplayName);
				Image_Entry->SetBrushFromTexture(ItemTemplate->IconTexture, true);
			}
		}
		break;
	case EL1CheatEntryType::Animation:
		AnimMontage = InAnimMontage;
		if (AnimMontage.IsNull() == false)
		{
			Text_Entry->SetText(FText::FromString(AnimMontage.GetAssetName()));
			SizeBox_Entry->SetVisibility(ESlateVisibility::Collapsed);
		}
		break;
	}
}

void UL1CheatEntryWidget::OnButtonClicked()
{
	ALyraPlayerController* LyraPlayerController = GetOwningPlayer<ALyraPlayerController>();
	if (LyraPlayerController == nullptr)
		return;
	
	// SSG: 
	/*switch (CheatEntryType)
	{
	case EL1CheatEntryType::PrimaryWeapon:
		LyraPlayerController->Server_EquipWeapon(EWeaponSlotType::Primary, ItemTemplateClass);
		break;
	case EL1CheatEntryType::SecondaryWeapon:
		LyraPlayerController->Server_EquipWeapon(EWeaponSlotType::Secondary, ItemTemplateClass);
		break;
	case EL1CheatEntryType::Armor:
		LyraPlayerController->Server_EquipArmor(ItemTemplateClass);
		break;
	case EL1CheatEntryType::Utility:
		LyraPlayerController->Server_EquipUtility(EUtilitySlotType::Primary, ItemTemplateClass);
		break;
	case EL1CheatEntryType::Animation:
		if (ULyraCheatManager* LyraCheatManager = Cast<ULyraCheatManager>(LyraPlayerController->CheatManager))
		{
			LyraCheatManager->SelectedMontage = AnimMontage;
		}
		break;
	}*/
}