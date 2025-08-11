#pragma once

#include "L1Define.h"
#include "UObject/Interface.h"
#include "L1Pickupable.generated.h"

class UL1ItemTemplate;
class UL1ItemInstance;

USTRUCT(BlueprintType)
struct FL1PickupTemplate
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UL1ItemTemplate> ItemTemplateClass;

	UPROPERTY(EditAnywhere)
	EItemRarity ItemRarity = EItemRarity::Poor;
	
	UPROPERTY(EditAnywhere)
	int32 ItemCount = 1;
};

USTRUCT(BlueprintType)
struct FL1PickupInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UL1ItemInstance> ItemInstance;

	UPROPERTY(EditAnywhere)
	int32 ItemCount = 1;
};

USTRUCT(BlueprintType)
struct FL1PickupInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FL1PickupTemplate PickupTemplate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FL1PickupInstance PickupInstance;
};

UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UL1Pickupable : public UInterface
{
	GENERATED_BODY()
};

class IL1Pickupable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual FL1PickupInfo GetPickupInfo() const = 0;
};
