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

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEquipStateChanged, EEquipState, EEquipState);

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
	UL1EquipManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;

public:
	void Equip(EEquipmentSlotType EquipmentSlotType, UL1ItemInstance* ItemInstance);
	void Unequip(EEquipmentSlotType EquipmentSlotType);

	void EquipCurrentSlots();
	void UnequipCurrentSlots();

public:
	UFUNCTION(BlueprintCallable)
	void ChangeEquipState(EEquipState NewEquipState);

	UFUNCTION(BlueprintCallable)
	bool CanChangeEquipState(EEquipState NewEquipState) const;

private:
	UFUNCTION()
	void CurrentEquip(EEquipState PrevEquipState);

	void BroadcastChangedMessage(EEquipState PrevEquipState, EEquipState NewEquipState);
public:
	ALyraCharacter* GetCharacter() const;
	ALyraPlayerController* GetPlayerController() const;
	
	TArray<FL1EquipEntry>& GetEntries();
	UAbilitySystemComponent* GetAbilitySystemComponent() const;
	UL1EquipmentManagerComponent* GetEquipmentManager() const;

	static EEquipmentSlotType ConvertToEquipmentSlotType(EWeaponHandType WeaponHandType, EEquipState EquipState);
	static EEquipmentSlotType ConvertToEquipmentSlotType(EWeaponHandType WeaponHandType, EWeaponSlotType WeaponSlotType);
	static EEquipmentSlotType ConvertToEquipmentSlotType(EArmorType ArmorType);
	static EEquipmentSlotType ConvertToEquipmentSlotType(EUtilitySlotType UtilitySlotType);

	static EWeaponSlotType ConvertToWeaponSlotType(EEquipmentSlotType EquipmentSlotType);
	static EWeaponHandType ConvertToWeaponHandType(EEquipmentSlotType EquipmentSlotType);
	static EArmorType ConvertToArmorType(EEquipmentSlotType EquipmentSlotType);
	static EUtilitySlotType ConvertToUtilitySlotType(EEquipmentSlotType EquipmentSlotType);
	static EEquipState ConvertToEquipState(EWeaponSlotType WeaponSlotType);
	static EEquipState ConvertToEquipState(EUtilitySlotType UtilitySlotType);

	static bool IsWeaponEquipState(EEquipState EquipState);
	static bool IsUtilityEquipState(EEquipState EquipState);

	static const TArray<EEquipmentSlotType>& GetEquipmentSlotsByEquipState(EEquipState EquipState);

	UFUNCTION(BlueprintCallable)
	void ChangeShouldHiddenEquipments(bool bNewShouldHiddenEquipments);

	bool ShouldHiddenEquipments() const { return bShouldHiddenEquipments; }
	EEquipState GetCurrentEquipState() const { return CurrentEquipState; }

	AL1EquipmentBase* GetFirstEquippedActor() const;
	AL1EquipmentBase* GetEquippedActor(EWeaponHandType WeaponHandType) const;
	void GetAllEquippedActors(TArray<AL1EquipmentBase*>& OutActors) const;

	UL1ItemInstance* GetFirstEquippedItemInstance(bool bIgnoreArmor = true) const;
	UL1ItemInstance* GetEquippedItemInstance(EArmorType ArmorType) const;
	UL1ItemInstance* GetEquippedItemInstance(EWeaponHandType WeaponHandType) const;
	UL1ItemInstance* GetEquippedItemInstance(EEquipmentSlotType EquipmentSlotType) const;

public:
	FOnEquipStateChanged OnEquipStateChanged;

private:
	UPROPERTY()
	TArray<FL1EquipEntry> Entries;

	UPROPERTY()
	EEquipState CurrentEquipState = EEquipState::Count;

	UPROPERTY()
	bool bShouldHiddenEquipments = false;
};