#pragma once

#include "UI/L1ActivatableWidget.h"
#include "L1ClassSelectionWidget.generated.h"

class UButton;
class UVerticalBox;
class UL1ClassEntryWidget;

UCLASS()
class UL1ClassSelectionWidget : public UL1ActivatableWidget
{
	GENERATED_BODY()
	
public:
	UL1ClassSelectionWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnExitButtonClicked();

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UL1ClassEntryWidget> ClassEntryWidgetClass;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox_ClassElements;
};
