#pragma once

#include "Blueprint/UserWidget.h"
#include "L1PocketWorldWidget.generated.h"

class UImage;

UCLASS()
class UL1PocketWorldWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UL1PocketWorldWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;

private:
	void RefreshRenderTarget();

private:
	UFUNCTION()
	void OnPocketStageReady(AL1PocketStage* PocketStage);
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Render;

private:
	UPROPERTY()
	TWeakObjectPtr<AL1PocketStage> CachedPocketStage;

private:
	FDelegateHandle DelegateHandle;
};
