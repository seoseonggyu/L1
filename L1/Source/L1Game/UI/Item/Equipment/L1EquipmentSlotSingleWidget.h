#pragma once

#include "L1Define.h"
#include "L1EquipmentSlotWidget.h"
#include "L1EquipmentSlotSingleWidget.generated.h"

class UImage;
class UOverlay;
class UL1ItemInstance;
class UL1EquipmentEntryWidget;
class UL1EquipmentManagerComponent;

UENUM()
enum class EEquipmentSingleSlotType : uint8
{
	None,
	Armor,
	Utility,
};

UCLASS()
class UL1EquipmentSlotSingleWidget : public UL1EquipmentSlotWidget
{
	GENERATED_BODY()
	
public:
	UL1EquipmentSlotSingleWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void Init(EArmorType InArmorType, UL1EquipmentManagerComponent* InEquipmentManager);
	void Init(EUtilitySlotType InUtilitySlotType, UL1EquipmentManagerComponent* InEquipmentManager);
	
protected:
	virtual void NativePreConstruct() override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void OnDragEnded() override;

public:
	void OnEquipmentEntryChanged(UL1ItemInstance* InItemInstance, int32 InItemCount);

private:
	EEquipmentSlotType GetEquipmentSlotType() const;
	
protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> BaseIconTexture;
	
private:
	UPROPERTY()
	TObjectPtr<UL1EquipmentEntryWidget> EntryWidget;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_Entry;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_BaseIcon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Red;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Green;

private:
	EEquipmentSingleSlotType EquipmentSingleSlotType = EEquipmentSingleSlotType::None;
	EArmorType ArmorType = EArmorType::Count;
	EUtilitySlotType UtilitySlotType = EUtilitySlotType::Count;
};
