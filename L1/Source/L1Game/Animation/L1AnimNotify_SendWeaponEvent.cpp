#include "L1AnimNotify_SendWeaponEvent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Actors/L1EquipmentBase.h"
#include "Character/LyraCharacter.h"
#include "Item/Managers/L1EquipManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1AnimNotify_SendWeaponEvent)

UL1AnimNotify_SendWeaponEvent::UL1AnimNotify_SendWeaponEvent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif
	bIsNativeBranchingPoint = true;
}

void UL1AnimNotify_SendWeaponEvent::Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComponent, Animation, EventReference);

	if (ALyraCharacter* Character = Cast<ALyraCharacter>(MeshComponent->GetOwner()))
	{
		if (UL1EquipManagerComponent* EquipManager = Character->FindComponentByClass<UL1EquipManagerComponent>())
		{
			AL1EquipmentBase* WeaponActor = EquipManager->GetEquippedActor(WeaponHandType);
			if (WeaponActor && EventData.EventTag.IsValid())
			{
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(WeaponActor, EventData.EventTag, EventData);
			}
		}
	}
}
