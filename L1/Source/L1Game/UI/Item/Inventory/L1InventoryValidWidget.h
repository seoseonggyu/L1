#pragma once

#include "L1Define.h"
#include "Blueprint/UserWidget.h"
#include "L1InventoryValidWidget.generated.h"

class UImage;
class USizeBox;

UCLASS()
class UL1InventoryValidWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UL1InventoryValidWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeOnInitialized() override;

public:
	void ChangeSlotState(ESlotState SlotState);
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> SizeBox_Root;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Red;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Green;
};
