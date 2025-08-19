#pragma once

#include "L1Define.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "L1AnimNotify_SendWeaponEvent.generated.h"

class AL1EquipmentBase;

UCLASS(meta=(DisplayName="Send Weapon Event"))
class UL1AnimNotify_SendWeaponEvent : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UL1AnimNotify_SendWeaponEvent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere)
	EWeaponHandType WeaponHandType = EWeaponHandType::LeftHand;
	
	UPROPERTY(EditAnywhere)
	FGameplayEventData EventData;
};
