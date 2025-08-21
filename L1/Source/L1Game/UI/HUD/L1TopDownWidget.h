#pragma once

#include "Blueprint/UserWidget.h"
#include "L1TopDownWidget.generated.h"

class ALyraCharacter;

UCLASS()
class L1GAME_API UL1TopDownWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetCharacter(ALyraCharacter* InCharacter);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UPROPERTY(BlueprintReadOnly, Category = "L1", meta = (AllowPrivateAccess = "true"))
	float TargetValue;
	UPROPERTY(BlueprintReadOnly, Category = "L1", meta = (AllowPrivateAccess = "true"))
	float MaxValue;

	ALyraCharacter* CachedCharacter = nullptr;
};
