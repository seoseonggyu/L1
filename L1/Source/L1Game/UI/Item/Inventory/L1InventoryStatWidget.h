#pragma once

#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "L1InventoryStatWidget.generated.h"

class UL1SkillStatHoverWidget;

UCLASS()
class UL1InventoryStatWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UL1InventoryStatWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeDestruct() override;
	
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

protected:
	UPROPERTY(EditAnywhere)
	FGameplayTag StatTag;
	
protected:
	UPROPERTY()
	TObjectPtr<UL1SkillStatHoverWidget> HoverWidget;
};
