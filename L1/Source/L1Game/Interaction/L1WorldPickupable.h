#pragma once

#include "L1Pickupable.h"
#include "L1WorldInteractable.h"
#include "L1WorldPickupable.generated.h"

UCLASS(Abstract, Blueprintable)
class AL1WorldPickupable : public AActor, public IL1Interactable, public IL1Pickupable
{
	GENERATED_BODY()
	
public:
	AL1WorldPickupable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual FL1InteractionInfo GetPreInteractionInfo(const FL1InteractionQuery& InteractionQuery) const override { return InteractionInfo; }
	virtual void SetPickupInfo(const FL1PickupInfo& InPickupInfo);
	virtual FL1PickupInfo GetPickupInfo() const override { return PickupInfo; }

protected:
	UFUNCTION()
	virtual void ProcessPickup();

protected:
	UPROPERTY(EditAnywhere, Category="Info")
	FL1InteractionInfo InteractionInfo;
	
	UPROPERTY(EditAnywhere, Category="Info")
	FL1PickupInfo PickupInfo;
};
