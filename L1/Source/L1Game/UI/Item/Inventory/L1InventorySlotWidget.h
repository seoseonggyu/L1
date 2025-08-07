#pragma once

#include "Blueprint/UserWidget.h"
#include "L1InventorySlotWidget.generated.h"

class UImage;
class USizeBox;

UCLASS()
class UL1InventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UL1InventorySlotWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeOnInitialized() override;

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> SizeBox_Root;
};
