#pragma once

#include "L1Define.h"
#include "GameplayTagContainer.h"
#include "L1UIData.generated.h"

class UImage;
class UL1SkillStatHoverWidget;
class UL1InventoryValidWidget;
class UL1ItemDragWidget;
class UL1ItemHoverWidget;
class UL1EquipmentEntryWidget;
class UL1InventorySlotWidget;
class UL1InventoryEntryWidget;

USTRUCT(BlueprintType)
struct FL1ItemRarityInfoEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, meta=(HideAlphaChannel))
	FColor Color = FColor::Black;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> EntryTexture;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> HoverTexture;
};

USTRUCT(BlueprintType)
struct FL1UIInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> Icon;
	
	UPROPERTY(EditDefaultsOnly)
	FText Title;

	UPROPERTY(EditDefaultsOnly)
	FText Content;
};

UCLASS(BlueprintType, Const, meta=(DisplayName="L1 UI Data"))
class UL1UIData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	static const UL1UIData& Get();
	
public:
	UTexture2D* GetEntryRarityTexture(EItemRarity ItemRarity) const;
	UTexture2D* GetHoverRarityTexture(EItemRarity ItemRarity) const;
	FColor GetRarityColor(EItemRarity ItemRarity) const;

	const FL1UIInfo& GetTagUIInfo(FGameplayTag Tag) const;

public:
	UPROPERTY(EditDefaultsOnly)
	FIntPoint UnitInventorySlotSize = FIntPoint::ZeroValue;
	
public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UL1ItemDragWidget> DragWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UL1ItemHoverWidget> ItemHoverWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UL1SkillStatHoverWidget> SkillStatHoverWidget;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UL1EquipmentEntryWidget> EquipmentEntryWidgetClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UL1InventorySlotWidget> InventorySlotWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UL1InventoryEntryWidget> InventoryEntryWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UL1InventoryValidWidget> InventoryValidWidgetClass;
	
private:
	UPROPERTY(EditDefaultsOnly, meta=(ArraySizeEnum="EItemRarity"))
	FL1ItemRarityInfoEntry RarityInfoEntries[(int32)EItemRarity::Count];

	UPROPERTY(EditDefaultsOnly, meta=(DisplayName="Tag UI Infos"))
	TMap<FGameplayTag, FL1UIInfo> TagUIInfos;
};
