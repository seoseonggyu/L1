#include "L1SkillInputWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1SkillInputWidget)

UL1SkillInputWidget::UL1SkillInputWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UL1SkillInputWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Collapsed);
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	ListenerHandle = MessageSubsystem.RegisterListener(MessageChannelTag, this, &ThisClass::ConstructUI);
}

void UL1SkillInputWidget::NativeDestruct()
{
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.UnregisterListener(ListenerHandle);
	
	Super::NativeDestruct();
}

void UL1SkillInputWidget::ConstructUI(FGameplayTag Channel, const FSkillInputInitializeMessage& Message)
{
	Message.bShouldShow ? SetVisibility(ESlateVisibility::Visible) : SetVisibility(ESlateVisibility::Hidden);
}
