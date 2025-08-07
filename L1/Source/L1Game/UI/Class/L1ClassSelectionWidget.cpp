#include "L1ClassSelectionWidget.h"

#include "L1ClassEntryWidget.h"
#include "Components/VerticalBox.h"
#include "Data/L1ClassData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1ClassSelectionWidget)

UL1ClassSelectionWidget::UL1ClassSelectionWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UL1ClassSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	VerticalBox_ClassElements->ClearChildren();

	const int32 ClassCount = (int32)ECharacterClassType::Count;
	for (int i = 0; i < ClassCount; i++)
	{
		UL1ClassEntryWidget* ClassEntryWidget = CreateWidget<UL1ClassEntryWidget>(this, ClassEntryWidgetClass);
		ClassEntryWidget->InitializeUI(this, (ECharacterClassType)i);
		VerticalBox_ClassElements->AddChild(ClassEntryWidget);
	}
}

void UL1ClassSelectionWidget::OnExitButtonClicked()
{
	DeactivateWidget();
}
