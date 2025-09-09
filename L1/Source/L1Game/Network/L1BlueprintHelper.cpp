#include "Network/L1BlueprintHelper.h"
#include "Item/Managers/L1EquipmentManagerComponent.h"
#include "Item/Managers/L1InventoryManagerComponent.h"
#include "Character/LyraCharacter.h"

UL1InventoryManagerComponent* UL1BlueprintHelper::GetCharacterInventoryManager(ALyraCharacter* LyraCharacter)
{
	UL1InventoryManagerComponent* MyInventoryManager = nullptr;
	if (LyraCharacter)
	{
		MyInventoryManager = LyraCharacter->GetComponentByClass<UL1InventoryManagerComponent>();
	}

	return MyInventoryManager;
}

UL1EquipmentManagerComponent* UL1BlueprintHelper::GetCharacterEquipmentManager(ALyraCharacter* LyraCharacter)
{
	UL1EquipmentManagerComponent* MyEquipmentManager = nullptr;
	if (LyraCharacter)
	{
		MyEquipmentManager = LyraCharacter->GetComponentByClass<UL1EquipmentManagerComponent>();
	}

	return MyEquipmentManager;
}