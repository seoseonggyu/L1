#pragma once

#include "Interaction/L1WorldPickupable.h"
#include "L1PickupableItemBase.generated.h"

class UBoxComponent;
class UProjectileMovementComponent;

UCLASS()
class AL1PickupableItemBase : public AL1WorldPickupable
{
	GENERATED_BODY()
	
public:
	AL1PickupableItemBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ProcessPickup() override;
	virtual void GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const override;

protected:
	UPROPERTY(EditDefaultsOnly)
	bool bAutoCollisionResize = true;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> MovementCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> PickupCollision;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

protected:
	UPROPERTY(EditDefaultsOnly)
	FVector2D MaxMovementCollisionExtent = FVector2D(16.f, 16.f);

	UPROPERTY(EditDefaultsOnly)
	FVector2D MinPickupCollisionExtent = FVector2D(32.f, 32.f);
};
