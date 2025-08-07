#pragma once

#include "Item/L1ItemInstance.h"
#include "UI/L1HoverWidget.h"
#include "L1ItemHoverWidget.generated.h"

class UL1EquipmentManagerComponent;
class UL1ItemInstance;
class UCanvasPanel;
class UHorizontalBox;
class UL1ItemHoverEntryWidget;

UCLASS()
class UL1ItemHoverWidget : public UL1HoverWidget
{
	GENERATED_BODY()
	
public:
	UL1ItemHoverWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void RefreshUI(const UL1ItemInstance* HoveredItemInstance);
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UL1ItemHoverEntryWidget> HoverWidget_Left;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UL1ItemHoverEntryWidget> HoverWidget_Right;
};
