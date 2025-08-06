#pragma once

#include "L1Define.h"
#include "AbilitySystem/LyraAbilitySet.h"
#include "Components/PawnComponent.h"
#include "L1EquipManagerComponent.generated.h"

class ALyraCharacter;
class ALyraPlayerController;
class AL1EquipmentBase;
class UL1ItemInstance;
class UL1EquipManagerComponent;
class UL1EquipmentManagerComponent;

USTRUCT(BlueprintType)
struct FL1EquipEntry
{
	GENERATED_BODY()

private:
	void Init(UL1ItemInstance* InItemInstance);

	void Equip();
	void Unequip();

public:
	UL1ItemInstance* GetItemInstance() const { return ItemInstance; }

	void SetEquipmentActor(AL1EquipmentBase* InEquipmentActor) { SpawnedEquipmentActor = InEquipmentActor; }
	AL1EquipmentBase* GetEquipmentActor() const { return SpawnedEquipmentActor; }

private:
	friend class UL1EquipManagerComponent;

	UPROPERTY()
	TObjectPtr<UL1ItemInstance> ItemInstance;

private:
	UPROPERTY()
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

private:
	UPROPERTY()
	TObjectPtr<AL1EquipmentBase> SpawnedEquipmentActor;

	UPROPERTY()
	TObjectPtr<AL1EquipmentBase> SpawnedPocketWorldActor;

private:
	UPROPERTY()
	FLyraAbilitySet_GrantedHandles BaseAbilitySetHandles;

	UPROPERTY()
	FActiveGameplayEffectHandle BaseStatHandle;

public:
	UPROPERTY()
	TObjectPtr<UL1EquipManagerComponent> EquipManager;
};



UCLASS(BlueprintType)
class UL1EquipManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	ALyraCharacter* GetCharacter() const;
	ALyraPlayerController* GetPlayerController() const;

	UAbilitySystemComponent* GetAbilitySystemComponent() const;

	bool ShouldHiddenEquipments() const { return bShouldHiddenEquipments; }

private:
	UPROPERTY()
	bool bShouldHiddenEquipments = false;
};