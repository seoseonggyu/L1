#pragma once

#include "L1Define.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "L1EquipmentSlotsWidget.generated.h"

class UCommonVisibilitySwitcher;
class UL1ItemInstance;
class UL1ItemSlotWidget;
class UL1EquipmentSlotWeaponWidget;
class UL1EquipmentSlotSingleWidget;
class UL1EquipManagerComponent;
class UL1EquipmentManagerComponent;

USTRUCT(BlueprintType)
struct FEquipmentInitializeMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UL1EquipmentManagerComponent> EquipmentManager;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UL1EquipManagerComponent> EquipManager;
};

UCLASS()
class UL1EquipmentSlotsWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UL1EquipmentSlotsWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
private:
	void ConstructUI(FGameplayTag Channel, const FEquipmentInitializeMessage& Message);
	void DestructUI();
	
	void OnEquipmentEntryChanged(EEquipmentSlotType EquipmentSlotType, UL1ItemInstance* ItemInstance, int32 ItemCount);
	void OnEquipStateChanged(EEquipState PrevEquipState, EEquipState NewEquipState);

public:
	UPROPERTY(EditAnywhere, meta=(Categories="Message"))
	FGameplayTag MessageChannelTag;
	
private:
	UPROPERTY()
	TArray<TObjectPtr<UL1EquipmentSlotWeaponWidget>> WeaponSlotWidgets;

	UPROPERTY()
	TArray<TObjectPtr<UL1EquipmentSlotSingleWidget>> ArmorSlotWidgets;

	UPROPERTY()
	TArray<TObjectPtr<UL1EquipmentSlotSingleWidget>> UtilitySlotWidgets;
	
	UPROPERTY()
	TObjectPtr<UL1EquipmentManagerComponent> EquipmentManager;
	
	UPROPERTY()
	TObjectPtr<UL1EquipManagerComponent> EquipManager;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UL1EquipmentSlotWeaponWidget> Widget_Weapon_Primary;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UL1EquipmentSlotWeaponWidget> Widget_Weapon_Secondary;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UL1EquipmentSlotSingleWidget> Widget_Armor_Head;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UL1EquipmentSlotSingleWidget> Widget_Armor_Chest;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UL1EquipmentSlotSingleWidget> Widget_Armor_Legs;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UL1EquipmentSlotSingleWidget> Widget_Armor_Hand;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UL1EquipmentSlotSingleWidget> Widget_Armor_Foot;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UL1EquipmentSlotSingleWidget> Widget_Utility_Primary;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UL1EquipmentSlotSingleWidget> Widget_Utility_Secondary;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UL1EquipmentSlotSingleWidget> Widget_Utility_Tertiary;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UL1EquipmentSlotSingleWidget> Widget_Utility_Quaternary;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonVisibilitySwitcher> Switcher_Weapon_Primary;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonVisibilitySwitcher> Switcher_Weapon_Secondary;

private:
	FDelegateHandle EntryChangedDelegateHandle;
	FDelegateHandle EquipStateChangedDelegateHandle;
	FGameplayMessageListenerHandle MessageListenerHandle;
};
