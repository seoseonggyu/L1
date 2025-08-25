#pragma once

#include "L1Define.h"
#include "Components/PawnComponent.h"

#include "L1EquipmentManagerComponent.generated.h"

class UL1ItemInstance;
class UL1ItemTemplate;
class UL1EquipManagerComponent;
class UL1InventoryManagerComponent;
class ALyraCharacter;
class ALyraPlayerController;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnEquipmentEntryChanged, EEquipmentSlotType, UL1ItemInstance*, int32/*ItemCount*/);

USTRUCT(BlueprintType)
struct FL1EquipmentEntry
{
	GENERATED_BODY()

private:
	void Init(UL1ItemInstance* InItemInstance, int32 InItemCount);
	UL1ItemInstance* Reset();

public:
	UL1ItemInstance* GetItemInstance() const { return ItemInstance; }
	int32 GetItemCount() const { return ItemCount; }

private:
	friend struct FL1EquipmentList;
	friend class UL1EquipmentManagerComponent;

	UPROPERTY()
	TObjectPtr<UL1ItemInstance> ItemInstance;

	UPROPERTY()
	int32 ItemCount = 0;

private:
	UPROPERTY()
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

	UPROPERTY()
	TObjectPtr<UL1EquipmentManagerComponent> EquipmentManager;
};


UCLASS(BlueprintType)
class UL1EquipmentManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UL1EquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void InitializeComponent() override;

private:
	void BroadcastChangedMessage(EEquipmentSlotType EquipmentSlotType, UL1ItemInstance* ItemInstance, int32 ItemCount);

public:
	int32 CanMoveOrMergeEquipment(UL1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType ToEquipmentSlotType) const;
	int32 CanMoveOrMergeEquipment(UL1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, EEquipmentSlotType ToEquipmentSlotType) const;

	int32 CanMoveOrMergeEquipment_Quick(UL1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType& OutToEquipmentSlotType) const;
	int32 CanMoveOrMergeEquipment_Quick(UL1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, EEquipmentSlotType& OutToEquipmentSlotType) const;
	int32 CanMoveOrMergeEquipment_Quick(int32 FromItemTemplateID, EItemRarity FromItemRarity, int32 FromItemCount, EEquipmentSlotType& OutToEquipmentSlotType) const;

	bool CanSwapEquipment(UL1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType ToEquipmentSlotType) const;
	bool CanSwapEquipment(UL1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, EEquipmentSlotType ToEquipmentSlotType, FIntPoint& OutToItemSlotPos);

	bool CanSwapEquipment_Quick(UL1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, EEquipmentSlotType& OutToEquipmentSlotType) const;
	bool CanSwapEquipment_Quick(UL1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, EEquipmentSlotType& OutToEquipmentSlotType, FIntPoint& OutToItemSlotPos);

	int32 CanAddEquipment(int32 ItemTemplateID, EItemRarity ItemRarity, int32 ItemCount, EEquipmentSlotType ToEquipmentSlotType) const;

public:
	UFUNCTION(BlueprintCallable)
	void AddUnarmedEquipments(TSubclassOf<UL1ItemTemplate> LeftHandClass, TSubclassOf<UL1ItemTemplate> RightHandClass);

public:
	void AddEquipment_Unsafe(EEquipmentSlotType EquipmentSlotType, UL1ItemInstance* ItemInstance, int32 ItemCount);
	UL1ItemInstance* RemoveEquipment_Unsafe(EEquipmentSlotType EquipmentSlotType, int32 ItemCount);
	void SetEquipment(EEquipmentSlotType EquipmentSlotType, TSubclassOf<UL1ItemTemplate> ItemTemplateClass, EItemRarity ItemRarity, int32 ItemCount);

public:
	static bool IsWeaponSlot(EEquipmentSlotType EquipmentSlotType);
	static bool IsArmorSlot(EEquipmentSlotType EquipmentSlotType);
	static bool IsUtilitySlot(EEquipmentSlotType EquipmentSlotType);

	static bool IsSameEquipState(EEquipmentSlotType EquipmentSlotType, EEquipState WeaponEquipState);
	static bool IsSameWeaponHandType(EEquipmentSlotType EquipmentSlotType, EWeaponHandType WeaponHandType);
	static bool IsSameArmorType(EEquipmentSlotType EquipmentSlotType, EArmorType ArmorType);

	static bool IsPrimaryWeaponSlot(EEquipmentSlotType EquipmentSlotType);
	static bool IsSecondaryWeaponSlot(EEquipmentSlotType EquipmentSlotType);

	const UL1ItemInstance* FindPairItemInstance(const UL1ItemInstance* InBaseItemInstance, EEquipmentSlotType& OutEquipmentSlotType) const;

	bool IsAllEmpty(EEquipState EquipState) const;

	ALyraCharacter* GetCharacter() const;
	ALyraPlayerController* GetPlayerController() const;
	UL1EquipManagerComponent* GetEquipManager() const;

	UL1ItemInstance* GetItemInstance(EEquipmentSlotType EquipmentSlotType) const;
	int32 GetItemCount(EEquipmentSlotType EquipmentSlotType) const;

	const TArray<FL1EquipmentEntry>& GetEntries() const;
	void GetAllWeaponItemInstances(TArray<UL1ItemInstance*>& OutItemInstances) const;

public:
	FOnEquipmentEntryChanged OnEquipmentEntryChanged;

private:
	friend class UL1ItemManagerComponent;
	friend class ULyraCheatManager;

private:
	UPROPERTY()
	TArray<FL1EquipmentEntry> Entries;
};