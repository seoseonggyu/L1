#include "L1AnimNotifyState_PlayWeaponSound.h"

#include "Actors/L1EquipmentBase.h"
#include "Character/LyraCharacter.h"
#include "Components/AudioComponent.h"
#include "Item/L1ItemInstance.h"
#include "Item/Fragments/L1ItemFragment_Equipable_Weapon.h"
#include "Item/Managers/L1EquipManagerComponent.h"
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1AnimNotifyState_PlayWeaponSound)

UL1AnimNotifyState_PlayWeaponSound::UL1AnimNotifyState_PlayWeaponSound(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UL1AnimNotifyState_PlayWeaponSound::NotifyBegin(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComponent, Animation, TotalDuration, EventReference);

	if (WeaponHandType == EWeaponHandType::Count || WeaponSoundType == EWeaponSoundType::None)
		return;

	ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(MeshComponent->GetOwner());
	if (LyraCharacter == nullptr)
		return;
	
	UL1EquipManagerComponent* EquipManager = LyraCharacter->FindComponentByClass<UL1EquipManagerComponent>();
	if (EquipManager == nullptr)
		return;
	
	UL1ItemInstance* ItemInstance = EquipManager->GetEquippedItemInstance(WeaponHandType);
	if (ItemInstance == nullptr)
		return;

	AL1EquipmentBase* WeaponActor = EquipManager->GetEquippedActor(WeaponHandType);
	if (WeaponActor == nullptr)
		return;
	
	const UL1ItemFragment_Equipable_Weapon* WeaponFragment = ItemInstance->FindFragmentByClass<UL1ItemFragment_Equipable_Weapon>();
	if (WeaponFragment == nullptr)
		return;

	USoundBase* SelectedSound = nullptr;
	
	switch (WeaponSoundType)
	{
	case EWeaponSoundType::Swing:	SelectedSound = WeaponFragment->AttackSwingSound;	break;
	case EWeaponSoundType::Custom:	SelectedSound = CustomSound;						break;
	}

	if (SelectedSound)
	{
		AudioComponent = UGameplayStatics::SpawnSoundAttached(SelectedSound, WeaponActor->MeshComponent, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true, 1.f, 1.f, 0.f);
	}
}

void UL1AnimNotifyState_PlayWeaponSound::NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (AudioComponent)
	{
		AudioComponent->DestroyComponent();
	}
	
	Super::NotifyEnd(MeshComponent, Animation, EventReference);
}
