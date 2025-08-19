#include "L1AnimNotify_WeaponNiagaraEffect.h"

#include "Actors/L1EquipmentBase.h"
#include "Character/LyraCharacter.h"
#include "Item/Managers/L1EquipManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1AnimNotify_WeaponNiagaraEffect)

UL1AnimNotify_WeaponNiagaraEffect::UL1AnimNotify_WeaponNiagaraEffect()
	: Super()
{
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif
}

void UL1AnimNotify_WeaponNiagaraEffect::Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	USkeletalMeshComponent* WeaponMeshComponent = GetWeaponMeshComponent(MeshComponent);
	Super::Notify(WeaponMeshComponent ? WeaponMeshComponent : MeshComponent, Animation, EventReference);
}

USkeletalMeshComponent* UL1AnimNotify_WeaponNiagaraEffect::GetWeaponMeshComponent(USkeletalMeshComponent* MeshComponent) const
{
	USkeletalMeshComponent* WeaponMeshComponent = nullptr;
	
	if (ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(MeshComponent->GetOwner()))
	{
		if (UL1EquipManagerComponent* EquipManager = LyraCharacter->FindComponentByClass<UL1EquipManagerComponent>())
		{
			if (AL1EquipmentBase* WeaponActor = EquipManager->GetEquippedActor(WeaponHandType))
			{
				WeaponMeshComponent = WeaponActor->MeshComponent;
			}
		}
	}

	return WeaponMeshComponent;
}
