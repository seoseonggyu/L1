#include "UI/HUD/L1TopDownWidget.h"
#include "Components/WidgetComponent.h"
#include "Character/LyraCharacter.h"

void UL1TopDownWidget::SetCharacter(ALyraCharacter* InCharacter)
{
    CachedCharacter = InCharacter;
}

void UL1TopDownWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UL1TopDownWidget::NativeDestruct()
{
    Super::NativeDestruct();

}

void UL1TopDownWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (CachedCharacter)
    {
        TargetValue = CachedCharacter->VitalInfo->hp();
        MaxValue = CachedCharacter->VitalInfo->max_hp();
    }
}