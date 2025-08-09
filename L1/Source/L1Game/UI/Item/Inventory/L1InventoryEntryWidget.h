#pragma once

#include "UI/Item/L1ItemEntryWidget.h"
#include "L1InventoryEntryWidget.generated.h"

class USizeBox;
class UL1ItemInstance;
class UL1InventorySlotsWidget;

UCLASS()
class UL1InventoryEntryWidget : public UL1ItemEntryWidget
{
	GENERATED_BODY()
	
public:
	UL1InventoryEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void Init(UL1InventorySlotsWidget* InSlotsWidget, UL1ItemInstance* InItemInstance, int32 InItemCount);

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

private:
	UPROPERTY()
	TObjectPtr<UL1InventorySlotsWidget> SlotsWidget;

	FIntPoint CachedFromSlotPos = FIntPoint::ZeroValue;
	FVector2D CachedDeltaWidgetPos = FVector2D::ZeroVector;
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> SizeBox_Root;

public:
	// SSG: 아이템 마우스 클릭 옮기기 임시
	void QuickFromInventory(class UL1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos);

	// SSG: 아이템 마우스 클릭 옮기기 임시
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UL1InventoryManagerComponent* GetMyInventoryManager() const;

	// SSG: 아이템 마우스 클릭 옮기기 임시
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UL1EquipmentManagerComponent* GetMyEquipmentManager() const;

};
