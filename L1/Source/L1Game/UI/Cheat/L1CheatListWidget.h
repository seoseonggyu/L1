#pragma once

#include "Blueprint/UserWidget.h"
#include "L1CheatListWidget.generated.h"

class UTextBlock;
class UL1CheatEntryWidget;
class UScrollBox;

UCLASS()
class UL1CheatListWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UL1CheatListWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

public:
	void AddEntry(UL1CheatEntryWidget* EntryWidget);

protected:
	UPROPERTY(EditAnywhere)
	FText TitleText;
	
public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Title;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_Entries;
};
