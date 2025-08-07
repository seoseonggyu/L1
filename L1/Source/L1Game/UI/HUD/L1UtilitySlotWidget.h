#pragma once

#include "L1Define.h"
#include "Blueprint/UserWidget.h"
#include "L1UtilitySlotWidget.generated.h"

class UL1ItemInstance;
class UTextBlock;
class UImage;
class UL1EquipmentManagerComponent;
class UL1EquipManagerComponent;

UCLASS()
class UL1UtilitySlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UL1UtilitySlotWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	void OnEquipmentEntryChanged(EEquipmentSlotType EquipmentSlotType, UL1ItemInstance* ItemInstance, int32 ItemCount);
	void OnEquipStateChanged(EEquipState PrevEquipState, EEquipState NewEquipState);
	
public:
	UPROPERTY(EditAnywhere)
	EUtilitySlotType WidgetUtilitySlotType = EUtilitySlotType::Count;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_SlotNumber;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Highlight;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Icon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Count;

	UPROPERTY(meta=(BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> Animation_Highlight_In;
	
private:
	UPROPERTY()
	TObjectPtr<UL1EquipmentManagerComponent> EquipmentManager;
	
	UPROPERTY()
	TObjectPtr<UL1EquipManagerComponent> EquipManager;
	
private:
	FDelegateHandle EntryChangedDelegateHandle;
	FDelegateHandle EquipStateChangedDelegateHandle;
};
