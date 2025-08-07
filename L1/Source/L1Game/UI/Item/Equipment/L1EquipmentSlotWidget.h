#pragma once

#include "Blueprint/UserWidget.h"
#include "L1EquipmentSlotWidget.generated.h"

class UL1EquipmentEntryWidget;
class UL1EquipmentManagerComponent;

UCLASS()
class UL1EquipmentSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UL1EquipmentSlotWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void OnDragEnded();

protected:
	UPROPERTY()
	TObjectPtr<UL1EquipmentManagerComponent> EquipmentManager;

protected:
	bool bAlreadyHovered = false;
};
