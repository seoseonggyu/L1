
#pragma once

#include "Components/ControllerComponent.h"
#include "L1Define.h"
#include "Protocol.pb.h"
#include "L1ItemManagerComponent.generated.h"


class ALyraCharacter;
class UL1EquipmentManagerComponent;
class UL1InventoryManagerComponent;
class UL1ItemInstance;
class UL1NetworkManager;

UCLASS()
class L1GAME_API UL1ItemManagerComponent : public UControllerComponent
{
	GENERATED_BODY()

public:	
	UL1ItemManagerComponent(const FObjectInitializer& ObjectInitializer);

public:
	void Check_EquipmentToInventory(UL1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UL1InventoryManagerComponent* ToInventoryManager, const FIntPoint& ToItemSlotPos);
	void Check_InventoryToInventory(UL1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, UL1InventoryManagerComponent* ToInventoryManager, const FIntPoint& ToItemSlotPos);
	void Check_InventoryToEquipment(UL1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos, UL1EquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType);
	void Check_EquipmentToEquipment(UL1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UL1EquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType);
	void Check_QuickFromInventory(ALyraCharacter* ToCharacter, UL1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos);
	void Check_QuickFromEquipment(ALyraCharacter* ToCharacter, UL1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType);
	void Check_DropItemFromInventory(UL1InventoryManagerComponent* FromInventoryManager, const FIntPoint& FromItemSlotPos);
	void Check_DropItemFromEquipment(UL1EquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType);

	UFUNCTION(BlueprintCallable)
	void EquipmentToInventory(ALyraCharacter* FromPlayer, EEquipmentSlotType FromEquipmentSlotType, ALyraCharacter* ToPlayer, const FIntPoint& ToItemSlotPos, int32 MovableCount);
	UFUNCTION(BlueprintCallable)
	void InventoryToInventory(ALyraCharacter* FromPlayer, const FIntPoint& FromItemSlotPos, ALyraCharacter* ToPlayer, const FIntPoint& ToItemSlotPos, int32 MovableCount);
	UFUNCTION(BlueprintCallable)
	void InventoryToEquipment(ALyraCharacter* FromPlayer, ALyraCharacter* ToPlayer, EEquipmentSlotType ToEquipmentSlotType, const FIntPoint& FromItemSlotPos, const FIntPoint& ToItemSlotPos, int32 MovableCount);
	UFUNCTION(BlueprintCallable)
	void EquipmentToEquipment(ALyraCharacter* FromPlayer, ALyraCharacter* ToPlayer, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType ToEquipmentSlotType, int32 MovableCount);
	UFUNCTION(BlueprintCallable)
	void QuickFromInventory(ALyraCharacter* FromPlayer, ALyraCharacter* ToPlayer, const FIntPoint& FromItemSlotPos);
	UFUNCTION(BlueprintCallable)
	void QuickFromEquipment(ALyraCharacter* FromPlayer, ALyraCharacter* ToPlayer, EEquipmentSlotType FromEquipmentSlotType);


	void TryDropItem(ALyraCharacter* ItemDropCharacter, int32 ItemId, const FIntPoint& ItemSlotPos, int32 ItemCount, Protocol::ItemTransferType ItemTrnsferType, EEquipmentSlotType EquipmentSlotType);
	bool TryDrop(ALyraCharacter* ItemDropCharacter, UL1ItemInstance* FromItemInstance, int32 FromItemCount);

private:
	void SetNetworkManager();

private:
	UL1NetworkManager* CachedNetworkManager;
};
