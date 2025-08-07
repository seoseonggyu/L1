#pragma once

#include "Blueprint/UserWidget.h"
#include "L1ItemHoverEntryWidget.generated.h"

class UImage;
class UTextBlock;
class UHorizontalBox;
class URichTextBlock;
class UL1ItemInstance;

UCLASS()
class UL1ItemHoverEntryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UL1ItemHoverEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());\
	
protected:
	virtual void NativePreConstruct() override;

public:
	void RefreshUI(const UL1ItemInstance* ItemInstance);

protected:
	UPROPERTY(EditAnywhere)
	bool bIsEquippedWidget = false;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_DisplayName;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_IsEquipped;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_ItemRarity;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_ItemType;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_AttributeModifiers;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_AdditionalAttributeModifiers;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Description;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<URichTextBlock> Text_UsableClassList;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Gold;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_DisplayName_Background;

	UPROPERTY(meta=(BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> Animation_FadeIn;
};
