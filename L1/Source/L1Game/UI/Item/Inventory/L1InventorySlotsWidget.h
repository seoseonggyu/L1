#pragma once

#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "L1InventorySlotsWidget.generated.h"

class UL1InventoryValidWidget;
class UOverlay;
class UTextBlock;
class UL1InventoryEntryWidget;
class UL1InventorySlotWidget;
class UL1InventoryManagerComponent;
class UUniformGridPanel;
class UCanvasPanel;
class UL1ItemInstance;

USTRUCT(BlueprintType)
struct FInventoryInitializeMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UL1InventoryManagerComponent> InventoryManager;
};

UCLASS()
class UL1InventorySlotsWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UL1InventorySlotsWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

private:
	void ConstructUI(FGameplayTag Channel, const FInventoryInitializeMessage& Message);
	void DestructUI();
	
	void ResetValidSlots();
	void FinishDrag();
	void OnInventoryEntryChanged(const FIntPoint& ItemSlotPos, UL1ItemInstance* ItemInstance, int32 InItemCount);

public:
	UL1InventoryManagerComponent* GetInventoryManager() const { return InventoryManager; }
	const FGeometry& GetSlotContainerGeometry() const;

public:
	UPROPERTY(EditAnywhere, meta=(Categories="Message"))
	FGameplayTag MessageChannelTag;

	UPROPERTY(EditAnywhere)
	FText TitleText;

private:
	UPROPERTY()
	TArray<TObjectPtr<UL1InventorySlotWidget>> SlotWidgets;

	UPROPERTY()
	TArray<TObjectPtr<UL1InventoryEntryWidget>> EntryWidgets;

	UPROPERTY()
	TArray<TObjectPtr<UL1InventoryValidWidget>> ValidWidgets;

	UPROPERTY()
	TObjectPtr<UL1InventoryManagerComponent> InventoryManager;
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Title;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_Slots;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UUniformGridPanel> GridPanel_Slots;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel_Entries;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UUniformGridPanel> GridPanel_ValidSlots;
	
private:
	FDelegateHandle EntryChangedDelegateHandle;
	FIntPoint PrevDragOverSlotPos = FIntPoint(-1, -1);
	FGameplayMessageListenerHandle MessageListenerHandle;
};
