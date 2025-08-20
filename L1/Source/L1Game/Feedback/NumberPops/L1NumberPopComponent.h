#pragma once

#include "Components/ControllerComponent.h"
#include "GameplayTagContainer.h"
#include "L1NumberPopComponent.generated.h"

class UWidgetComponent;
class UL1NumberPopWidget;
class UL1NumberPopStyle;

USTRUCT(BlueprintType)
struct FNumberPopRequest
{
	GENERATED_BODY()

public:
	FNumberPopRequest()
		: WorldLocation(ForceInitToZero) { }

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="L1|Number Pop")
	FVector WorldLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="L1|Number Pop")
	FGameplayTagContainer SourceTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="L1|Number Pop")
	FGameplayTagContainer TargetTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="L1|Number Pop")
	int32 NumberToDisplay = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="L1|Number Pop")
	bool bIsCriticalDamage = false;
};

USTRUCT()
struct FPooledNumberPopWidgetList
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	TArray<TObjectPtr<UWidgetComponent>> Widgets;
};

USTRUCT()
struct FLiveWidgetNumberPopEntry
{
	GENERATED_BODY()

public:
	FLiveWidgetNumberPopEntry() { }

	FLiveWidgetNumberPopEntry(UWidgetComponent* InWidgetComponent, FPooledNumberPopWidgetList* InPoolList, float InReleaseTime)
		: WidgetComponent(InWidgetComponent), PoolList(InPoolList), ReleaseTime(InReleaseTime) { }
	
public:
	UPROPERTY(Transient)
	TObjectPtr<UWidgetComponent> WidgetComponent = nullptr;

	FPooledNumberPopWidgetList* PoolList = nullptr;

	float ReleaseTime = 0.f;
};

// TODO: UControllerComponent가 아니라 Pawn으로 바꿔야 함
UCLASS(Blueprintable)
class UL1NumberPopComponent : public UControllerComponent
{
	GENERATED_BODY()

public:
	UL1NumberPopComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFUNCTION(BlueprintCallable)
	void AddNumberPop(const FNumberPopRequest& NewRequest);

protected:
	void ReleaseNextWidgets();

private:
	FLinearColor DetermineColor(const FNumberPopRequest& Request);
	TSubclassOf<UL1NumberPopWidget> DetermineWidgetClass(const FNumberPopRequest& Request);

protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UL1NumberPopStyle>> Styles;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UL1NumberPopWidget> DefaultWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float WidgetLifeSpan = 3.f;

protected:
	int32 DamageNumber = 0;

	UPROPERTY(Transient)
	TMap<TSubclassOf<UL1NumberPopWidget>, FPooledNumberPopWidgetList> PooledWidgetMap;

	UPROPERTY(Transient)
	TArray<FLiveWidgetNumberPopEntry> LiveWidgets;

	FTimerHandle ReleaseTimerHandle;
};
