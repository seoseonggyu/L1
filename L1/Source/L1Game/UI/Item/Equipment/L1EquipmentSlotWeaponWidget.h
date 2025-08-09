#pragma once

#include "L1Define.h"
#include "L1EquipmentSlotWidget.h"
#include "L1EquipmentSlotWeaponWidget.generated.h"

class UImage;
class UOverlay;
class UCommonVisibilitySwitcher;
class UL1ItemInstance;
class UL1EquipmentEntryWidget;
class UL1EquipmentManagerComponent;

UCLASS()
class UL1EquipmentSlotWeaponWidget : public UL1EquipmentSlotWidget
{
	GENERATED_BODY()
	
public:
	UL1EquipmentSlotWeaponWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void Init(EWeaponSlotType InWeaponSlotType, UL1EquipmentManagerComponent* InEquipmentManager);
	
protected:
	virtual void NativeOnInitialized() override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void OnDragEnded() override;

public:
	void OnEquipmentEntryChanged(EWeaponHandType InWeaponHandType, UL1ItemInstance* InItemInstance, int32 InItemCount);
	
private:
	UPROPERTY()
	TArray<TObjectPtr<UL1EquipmentEntryWidget>> EntryWidgets;
	
	UPROPERTY()
	TArray<TObjectPtr<UImage>> SlotRedImages;

	UPROPERTY()
	TArray<TObjectPtr<UImage>> SlotGreenImages;

	UPROPERTY()
	TArray<TObjectPtr<UOverlay>> SlotOverlays;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonVisibilitySwitcher> Switcher_WeaponHandEntry;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_LeftHandEntry;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_RightHandEntry;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_TwoHandEntry;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Red_LeftHand;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Green_LeftHand;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Red_RightHand;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Green_RightHand;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Red_TwoHand;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Green_TwoHand;

private:
	EWeaponSlotType WeaponSlotType = EWeaponSlotType::Count;

public:
	// SSG: 아이템 옮기기 테스트 함수
	UFUNCTION(BlueprintCallable)
	void InventoryToEquipment(UL1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, UL1EquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType);
	UFUNCTION(BlueprintCallable)
	void EquipmentToEquipment(UL1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UL1EquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType);

};
