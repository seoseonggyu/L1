#pragma once

#include "GameplayTagContainer.h"
#include "L1NumberPopStyle.generated.h"

class UL1NumberPopWidget;

UCLASS()
class UL1NumberPopStyle : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="NumberPop")
	FGameplayTagQuery MatchPattern;

public:
	UPROPERTY(EditDefaultsOnly, Category="NumberPop")
	bool bOverrideColor = false;
	
	UPROPERTY(EditDefaultsOnly, Category="NumberPop", meta=(EditCondition=bOverrideColor))
	FLinearColor Color = FLinearColor::White;

	UPROPERTY(EditDefaultsOnly, Category="NumberPop", meta=(EditCondition=bOverrideColor))
	FLinearColor CriticalColor = FLinearColor::Yellow;

public:
	UPROPERTY(EditDefaultsOnly, Category="NumberPop")
	bool bOverrideWidget = false;

	UPROPERTY(EditDefaultsOnly, Category="NumberPop", meta=(EditCondition=bOverrideWidget))
	TSubclassOf<UL1NumberPopWidget> WidgetClass;
};
