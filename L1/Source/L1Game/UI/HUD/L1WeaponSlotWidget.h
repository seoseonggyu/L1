#pragma once

#include "L1Define.h"
#include "Blueprint/UserWidget.h"
#include "Item/L1ItemInstance.h"
#include "L1WeaponSlotWidget.generated.h"

class UImage;
class UOverlay;
class UTextBlock;
class UCommonVisibilitySwitcher;
class UL1EquipManagerComponent;
class UL1EquipmentManagerComponent;

UCLASS()
class UL1WeaponSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UL1WeaponSlotWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	void OnEquipmentEntryChanged(EEquipmentSlotType EquipmentSlotType, UL1ItemInstance* ItemInstance, int32 ItemCount);
	void OnEquipStateChanged(EEquipState PrevEquipState, EEquipState NewEquipState);
	
public:
	UPROPERTY(EditAnywhere)
	EWeaponSlotType WidgetWeaponSlotType = EWeaponSlotType::Count;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonVisibilitySwitcher> Switcher_Slots;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_SlotNumber;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_OneSlot;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_OneSlot_Count;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_TwoSlot_Left;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_TwoSlot_Right;

private:
	UPROPERTY(meta=(BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> Animation_ExpandSlot;
	
	UPROPERTY(meta=(BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> Animation_ShowCrossLine;

private:
	UPROPERTY()
	TObjectPtr<UL1EquipmentManagerComponent> EquipmentManager;
	
	UPROPERTY()
	TObjectPtr<UL1EquipManagerComponent> EquipManager;
	
private:
	FDelegateHandle EntryChangedDelegateHandle;
	FDelegateHandle EquipStateChangedDelegateHandle;
};
