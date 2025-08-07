#pragma once

#include "Blueprint/UserWidget.h"
#include "L1HoverWidget.generated.h"

class UCanvasPanel;
class UHorizontalBox;

UCLASS()
class UL1HoverWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UL1HoverWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void SetPosition(const FVector2D& AbsolutePosition);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel_Root;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> HorizontalBox_Hovers;
};
