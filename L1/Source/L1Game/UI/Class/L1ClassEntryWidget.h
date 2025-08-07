#pragma once

#include "L1Define.h"
#include "Blueprint/UserWidget.h"
#include "L1ClassEntryWidget.generated.h"

class UImage;
class UButton;
class UTextBlock;
class UVerticalBox;
class UL1ClassSelectionWidget;
class UL1ClassSkillEntryWidget;

UCLASS()
class UL1ClassEntryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UL1ClassEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFUNCTION()
	void InitializeUI(UL1ClassSelectionWidget* OwnerWidget, ECharacterClassType ClassType);

private:
	UFUNCTION()
	void OnButtonClicked();
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UL1ClassSkillEntryWidget> SkillEntryWidgetClass;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Class;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_ClassName;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox_SkillElements;

private:
	UPROPERTY()
	ECharacterClassType CachedClassType = ECharacterClassType::Count;

	UPROPERTY()
	TWeakObjectPtr<UL1ClassSelectionWidget> CachedOwnerWidget;
};
