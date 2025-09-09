#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "L1Define.h"
#include "L1ItemTransferHelper.generated.h"

class ALyraCharacter;
class UL1EquipmentManagerComponent;
class UL1InventoryManagerComponent;

UCLASS()
class L1GAME_API UL1ItemTransferHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static void EquipmentToInventory(ALyraCharacter* FromPlayer, EEquipmentSlotType FromEquipmentSlotType, ALyraCharacter* ToPlayer, const FIntPoint& ToItemSlotPos, int32 MovableCount);
	UFUNCTION(BlueprintCallable)
	static void InventoryToInventory(ALyraCharacter* FromPlayer, const FIntPoint& FromItemSlotPos, ALyraCharacter* ToPlayer, const FIntPoint& ToItemSlotPos, int32 MovableCount);
	UFUNCTION(BlueprintCallable)
	static void InventoryToEquipment(ALyraCharacter* FromPlayer, ALyraCharacter* ToPlayer, EEquipmentSlotType ToEquipmentSlotType, const FIntPoint& FromItemSlotPos, const FIntPoint& ToItemSlotPos, int32 MovableCount);
	UFUNCTION(BlueprintCallable)
	static void EquipmentToEquipment(ALyraCharacter* FromPlayer, ALyraCharacter* ToPlayer, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType ToEquipmentSlotType, int32 MovableCount);
	UFUNCTION(BlueprintCallable)
	static void QuickFromInventory(ALyraCharacter* FromPlayer, ALyraCharacter* ToPlayer, const FIntPoint& FromItemSlotPos);
	UFUNCTION(BlueprintCallable)
	static void QuickFromEquipment(ALyraCharacter* FromPlayer, ALyraCharacter* ToPlayer, EEquipmentSlotType FromEquipmentSlotType);

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static UL1InventoryManagerComponent* GetCharacterInventoryManager(ALyraCharacter* LyraCharacter);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static UL1EquipmentManagerComponent* GetCharacterEquipmentManager(ALyraCharacter* LyraCharacter);
};