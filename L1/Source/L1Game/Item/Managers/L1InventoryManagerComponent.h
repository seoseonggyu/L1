#pragma once

#include "L1Define.h"
#include "L1InventoryManagerComponent.generated.h"

class UL1ItemInstance;
class UL1EquipmentManagerComponent;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnInventoryEntryChanged, const FIntPoint&/*ItemSlotPos*/, UL1ItemInstance*, int32/*ItemCount*/);


USTRUCT(BlueprintType)
struct FL1InventoryEntry
{
	GENERATED_BODY()

private:
	UL1ItemInstance* Init(int32 InItemTemplateID, int32 InItemCount, EItemRarity InItemRarity);
	void Init(UL1ItemInstance* InItemInstance, int32 InItemCount);
	UL1ItemInstance* Reset();

public:
	UL1ItemInstance* GetItemInstance() const { return ItemInstance; }
	int32 GetItemCount() const { return ItemCount; }

private:
	friend struct FL1InventoryList;
	friend class UL1InventoryManagerComponent;
	friend class UL1ItemManagerComponent;

	UPROPERTY()
	TObjectPtr<UL1ItemInstance> ItemInstance;

	UPROPERTY()
	int32 ItemCount = 0;
};

UCLASS(BlueprintType, Blueprintable)
class UL1InventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UL1InventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void InitializeComponent() override;

public:
	int32 CanMoveOrMergeItem(UL1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, const FIntPoint& ToItemSlotPos) const;
	int32 CanMoveOrMergeItem(UL1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, const FIntPoint& ToItemSlotPos) const;

	int32 CanMoveOrMergeItem_Quick(UL1InventoryManagerComponent* OtherComponent, const FIntPoint& FromItemSlotPos, TArray<FIntPoint>& OutToItemSlotPoses, TArray<int32>& OutToItemCounts) const;
	int32 CanMoveOrMergeItem_Quick(UL1EquipmentManagerComponent* OtherComponent, EEquipmentSlotType FromEquipmentSlotType, TArray<FIntPoint>& OutToItemSlotPoses, TArray<int32>& OutToItemCounts) const;

	int32 CanAddItem(int32 ItemTemplateID, EItemRarity ItemRarity, int32 ItemCount, TArray<FIntPoint>& OutToItemSlotPoses, TArray<int32>& OutToItemCounts) const;
	bool CanRemoveItem(int32 ItemTemplateID, int32 ItemCount, TArray<FIntPoint>& OutToItemSlotPoses, TArray<int32>& OutToItemCounts) const;

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	int32 TryAddItemByRarity(TSubclassOf<UL1ItemTemplate> ItemTemplateClass, EItemRarity ItemRarity, int32 ItemCount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	int32 TryAddItemByProbability(TSubclassOf<UL1ItemTemplate> ItemTemplateClass, int32 ItemCount, const TArray<FL1ItemRarityProbability>& ItemProbabilities);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	bool TryRemoveItem(int32 ItemTemplateID, int32 ItemCount);

private:
	void AddItem_Unsafe(const FIntPoint& ItemSlotPos, UL1ItemInstance* ItemInstance, int32 ItemCount);
	UL1ItemInstance* RemoveItem_Unsafe(const FIntPoint& ItemSlotPos, int32 ItemCount);

private:
	void MarkSlotChecks(TArray<bool>& InSlotChecks, bool bIsUsing, const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount) const;
	void MarkSlotChecks(bool bIsUsing, const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount);

public:
	bool IsEmpty(const TArray<bool>& InSlotChecks, const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount) const;
	bool IsEmpty(const FIntPoint& ItemSlotPos, const FIntPoint& ItemSlotCount) const;
	bool IsAllEmpty();

	UL1ItemInstance* GetItemInstance(const FIntPoint& ItemSlotPos) const;
	int32 GetItemCount(const FIntPoint& ItemSlotPos) const;

	const TArray<FL1InventoryEntry>& GetEntries() const;
	int32 GetTotalCountByID(int32 ItemTemplateID) const;
	FIntPoint GetInventorySlotCount() const { return InventorySlotCount; }
	TArray<bool>& GetSlotChecks() { return SlotChecks; }

public:
	FOnInventoryEntryChanged OnInventoryEntryChanged;

private:
	friend class UL1EquipmentManagerComponent;
	friend class UL1ItemManagerComponent;

	UPROPERTY()
	TArray<FL1InventoryEntry> Entries;

	UPROPERTY()
	TArray<bool> SlotChecks;

	FIntPoint InventorySlotCount = FIntPoint(10, 5);
};