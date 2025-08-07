#include "L1CheatListWidget.h"

#include "L1CheatEntryWidget.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1CheatListWidget)

UL1CheatListWidget::UL1CheatListWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UL1CheatListWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	Text_Title->SetText(TitleText);
}

void UL1CheatListWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ScrollBox_Entries->ClearChildren();
}

void UL1CheatListWidget::AddEntry(UL1CheatEntryWidget* EntryWidget)
{
	ScrollBox_Entries->AddChild(EntryWidget);
}
