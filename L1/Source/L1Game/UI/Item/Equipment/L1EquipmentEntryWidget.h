#pragma once

#include "L1Define.h"
#include "UI/Item/L1ItemEntryWidget.h"
#include "L1EquipmentEntryWidget.generated.h"

class UL1ItemInstance;
class UL1EquipmentManagerComponent;

UCLASS()
class UL1EquipmentEntryWidget : public UL1ItemEntryWidget
{
	GENERATED_BODY()
	
public:
	UL1EquipmentEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void Init(UL1ItemInstance* InItemInstance, int32 InItemCount, EEquipmentSlotType InEquipmentSlotType, UL1EquipmentManagerComponent* InEquipmentManager);
	
protected:
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected:
	UPROPERTY()
	TObjectPtr<UL1EquipmentManagerComponent> EquipmentManager;

	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

public:
	// SSG: 아이템 마우스 클릭 옮기기 임시
	void QuickFromEquipment(class UL1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType);

	// SSG: 아이템 마우스 클릭 옮기기 임시
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UL1InventoryManagerComponent* GetMyInventoryManager() const;

	// SSG: 아이템 마우스 클릭 옮기기 임시
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UL1EquipmentManagerComponent* GetMyEquipmentManager() const;
};
