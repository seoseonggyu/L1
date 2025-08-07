#pragma once

#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "Blueprint/UserWidget.h"
#include "L1InventorySkillWidget.generated.h"

class UImage;
class UL1SkillStatHoverWidget;

UCLASS()
class UL1InventorySkillWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UL1InventorySkillWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

protected:
	void InitializeUI();
	
private:
	UFUNCTION()
	void OnAbilitySystemInitialized();

	UFUNCTION()
	void OnAbilityChanged(FGameplayAbilitySpecHandle AbilitySpecHandle, bool bGiven);
	
	FGameplayAbilitySpec* FindAbilitySpecFromHandle(FGameplayAbilitySpecHandle AbilitySpecHandle);
	
protected:
	UPROPERTY(EditAnywhere)
	FGameplayTag SkillSlotTag;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Icon;

protected:
	UPROPERTY()
	TObjectPtr<UL1SkillStatHoverWidget> HoverWidget;

private:
	FGameplayAbilitySpecHandle CachedAbilitySpecHandle;
	FDelegateHandle AbilityDelegateHandle;
};
