#pragma once

#include "Blueprint/UserWidget.h"
#include "L1ItemDropWidget.generated.h"

UCLASS()
class UL1ItemDropWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UL1ItemDropWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
