#pragma once

#include "Blueprint/UserWidget.h"
#include "L1CheatEntryWidget.generated.h"

class USizeBox;
class UImage;
class UButton;
class UCommonTextBlock;
class UL1ItemTemplate;

UENUM(BlueprintType)
enum class EL1CheatEntryType : uint8
{
	None,
	PrimaryWeapon,
	SecondaryWeapon,
	Armor,
	Utility,
	Animation
};

UCLASS()
class UL1CheatEntryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UL1CheatEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeOnInitialized() override;
	
public:
	void InitializeUI(EL1CheatEntryType InCheatEntryType, TSubclassOf<UL1ItemTemplate> InItemTemplateClass, TSoftObjectPtr<UAnimMontage> InAnimMontage);

private:
	UFUNCTION()
	void OnButtonClicked();
	
public:
	UPROPERTY()
	EL1CheatEntryType CheatEntryType = EL1CheatEntryType::None;

	UPROPERTY()
	TSubclassOf<UL1ItemTemplate> ItemTemplateClass;

	UPROPERTY()
	TSoftObjectPtr<UAnimMontage> AnimMontage;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> SizeBox_Entry;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Entry;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Entry;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> Text_Entry;
};
