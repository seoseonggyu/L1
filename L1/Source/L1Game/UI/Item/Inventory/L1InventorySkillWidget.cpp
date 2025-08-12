#include "L1InventorySkillWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayAbilitySpec.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Character/LyraPawnExtensionComponent.h"
#include "Components/Image.h"
#include "Data/L1UIData.h"
#include "UI/Item/L1SkillStatHoverWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1InventorySkillWidget)

UL1InventorySkillWidget::UL1InventorySkillWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UL1InventorySkillWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ULyraPawnExtensionComponent* PawnExtensionComponent = ULyraPawnExtensionComponent::FindPawnExtensionComponent(GetOwningPlayerPawn()))
	{
		PawnExtensionComponent->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	}
}

void UL1InventorySkillWidget::NativeDestruct()
{
	if (ULyraAbilitySystemComponent* LyraASC = Cast<ULyraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn())))
	{
		LyraASC->AbilityChangedDelegate.Remove(AbilityDelegateHandle);
		AbilityDelegateHandle.Reset();
	}

	if (HoverWidget)
	{
		HoverWidget->RemoveFromParent();
		HoverWidget = nullptr;
	}
	
	Super::NativeDestruct();
}

void UL1InventorySkillWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(CachedAbilitySpecHandle))
	{
		if (ULyraGameplayAbility* Ability = Cast<ULyraGameplayAbility>(AbilitySpec->Ability))
		{
			if (HoverWidget == nullptr)
			{
				TSubclassOf<UL1SkillStatHoverWidget> HoverWidgetClass = UL1UIData::Get().SkillStatHoverWidget;
				HoverWidget = CreateWidget<UL1SkillStatHoverWidget>(GetOwningPlayer(), HoverWidgetClass);
			}

			if (HoverWidget)
			{
				HoverWidget->RefreshUI(Ability->Name, Ability->Description);
				HoverWidget->AddToViewport();
			}
		}
	}
}

FReply UL1InventorySkillWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseMove(InGeometry, InMouseEvent);

	if (HoverWidget)
	{
		HoverWidget->SetPosition(InMouseEvent.GetScreenSpacePosition());
		return FReply::Handled();
	}
	
	return Reply;
}

void UL1InventorySkillWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	if (HoverWidget)
	{
		HoverWidget->RemoveFromParent();
		HoverWidget = nullptr;
	}
}

void UL1InventorySkillWidget::InitializeUI()
{
	if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(CachedAbilitySpecHandle))
	{
		if (ULyraGameplayAbility* Ability = Cast<ULyraGameplayAbility>(AbilitySpec->Ability))
		{
			Image_Icon->SetBrushFromTexture(Ability->Icon);
		}
	}
}

FGameplayAbilitySpec* UL1InventorySkillWidget::FindAbilitySpecFromHandle(FGameplayAbilitySpecHandle AbilitySpecHandle)
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn()))
	{
		if (FGameplayAbilitySpec* AbilitySpec = ASC->FindAbilitySpecFromHandle(AbilitySpecHandle))
		{
			return AbilitySpec;
		}
	}
	return nullptr;
}

void UL1InventorySkillWidget::OnAbilitySystemInitialized()
{
	if (ULyraAbilitySystemComponent* LyraASC = Cast<ULyraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn())))
	{
		for (const FGameplayAbilitySpec& AbilitySpec : LyraASC->GetActivatableAbilities())
		{
			if (AbilitySpec.Ability->AbilityTags.HasTagExact(SkillSlotTag))
			{
				CachedAbilitySpecHandle = AbilitySpec.Handle;
				InitializeUI();
				break;
			}
		}
		
		AbilityDelegateHandle = LyraASC->AbilityChangedDelegate.AddUObject(this, &ThisClass::OnAbilityChanged);
	}
}

void UL1InventorySkillWidget::OnAbilityChanged(FGameplayAbilitySpecHandle AbilitySpecHandle, bool bGiven)
{
	FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(AbilitySpecHandle);
	if (AbilitySpec && AbilitySpec->Ability->AbilityTags.HasTagExact(SkillSlotTag))
	{
		CachedAbilitySpecHandle = bGiven ? AbilitySpec->Handle : FGameplayAbilitySpecHandle();
		InitializeUI();
	}
}
