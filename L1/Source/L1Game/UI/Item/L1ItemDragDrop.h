#pragma once

#include "L1Define.h"
#include "Blueprint/DragDropOperation.h"
#include "L1ItemDragDrop.generated.h"

class UL1ItemInstance;
class UL1ItemEntryWidget;
class UL1EquipmentManagerComponent;
class UL1InventoryManagerComponent;

UCLASS()
class UL1ItemDragDrop : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	UL1ItemDragDrop(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY()
	TObjectPtr<UL1InventoryManagerComponent> FromInventoryManager;

	FIntPoint FromItemSlotPos = FIntPoint::ZeroValue;

public:
	UPROPERTY()
	TObjectPtr<UL1EquipmentManagerComponent> FromEquipmentManager;

	EEquipmentSlotType FromEquipmentSlotType = EEquipmentSlotType::Count;

public:
	UPROPERTY()
	TObjectPtr<UL1InventoryManagerComponent> ToInventoryManager;

	FIntPoint ToItemSlotPos = FIntPoint::ZeroValue;

public:
	UPROPERTY()
	TObjectPtr<UL1EquipmentManagerComponent> ToEquipmentManager;

	EEquipmentSlotType ToEquipmentSlotType = EEquipmentSlotType::Count;
	
public:
	UPROPERTY()
	TObjectPtr<UL1ItemEntryWidget> FromEntryWidget;
	
	UPROPERTY()
	TObjectPtr<UL1ItemInstance> FromItemInstance;
	
	FVector2D DeltaWidgetPos = FVector2D::ZeroVector;
};
