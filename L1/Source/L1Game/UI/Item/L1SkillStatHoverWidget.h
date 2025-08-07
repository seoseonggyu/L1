#pragma once

#include "UI/L1HoverWidget.h"
#include "L1SkillStatHoverWidget.generated.h"

class UL1SkillStatHoverEntryWidget;

UCLASS()
class UL1SkillStatHoverWidget : public UL1HoverWidget
{
	GENERATED_BODY()
	
public:
	UL1SkillStatHoverWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void RefreshUI(FText Name, FText Description);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UL1SkillStatHoverEntryWidget> Widget_HoverEntry;
};
