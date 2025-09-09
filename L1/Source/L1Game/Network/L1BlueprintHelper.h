#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "L1Define.h"
#include "L1BlueprintHelper.generated.h"

class ALyraCharacter;
class UL1EquipmentManagerComponent;
class UL1InventoryManagerComponent;

UCLASS()
class L1GAME_API UL1BlueprintHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static UL1InventoryManagerComponent* GetCharacterInventoryManager(ALyraCharacter* LyraCharacter);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static UL1EquipmentManagerComponent* GetCharacterEquipmentManager(ALyraCharacter* LyraCharacter);
};