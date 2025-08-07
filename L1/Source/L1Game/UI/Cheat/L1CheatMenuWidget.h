#pragma once

#include "UI/L1ActivatableWidget.h"
#include "L1CheatMenuWidget.generated.h"

class UButton;
class UL1CheatEntryWidget;
class UL1CheatListWidget;

UCLASS()
class UL1CheatMenuWidget : public UL1ActivatableWidget
{
	GENERATED_BODY()
	
public:
	UL1CheatMenuWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	
private:
	UFUNCTION()
	void OnExitButtonClicked();

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UL1CheatEntryWidget> CheatEntryWidgetClass;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UL1CheatListWidget> CheatList_PrimaryWeapon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UL1CheatListWidget> CheatList_SecondaryWeapon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UL1CheatListWidget> CheatList_Utility;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UL1CheatListWidget> CheatList_Armor;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UL1CheatListWidget> CheatList_Animation;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Exit;
};
