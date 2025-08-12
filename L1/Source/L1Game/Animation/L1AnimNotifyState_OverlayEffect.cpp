﻿#include "L1AnimNotifyState_OverlayEffect.h"

#include "Actors/L1EquipmentBase.h"
#include "Character/LyraCharacter.h"
#include "Curves/CurveLinearColor.h"
#include "Item/Managers/L1EquipManagerComponent.h"
#include "Kismet/KismetMaterialLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1AnimNotifyState_OverlayEffect)

UL1AnimNotifyState_OverlayEffect::UL1AnimNotifyState_OverlayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif
}

void UL1AnimNotifyState_OverlayEffect::NotifyBegin(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComponent, Animation, TotalDuration, EventReference);
	
	if (OverlayTargetType == EOverlayTargetType::None)
		return;

	FOverlayEffectProgressInfo& NewProgressInfo = ProgressInfoMap.Add(MeshComponent);
	NewProgressInfo.OverlayMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(MeshComponent, OverlayMaterial);
	
	switch (OverlayTargetType)
	{
	case EOverlayTargetType::Weapon:
		ApplyWeaponMeshComponent(NewProgressInfo, MeshComponent);
		break;
                                      		
	case EOverlayTargetType::Character:
		ApplyCharacterMeshComponents(NewProgressInfo, MeshComponent);
		break;
                                      		
	case EOverlayTargetType::All:
		ApplyAllWeaponMeshComponents(NewProgressInfo, MeshComponent);
		ApplyCharacterMeshComponents(NewProgressInfo, MeshComponent);
		break;
	}
}

void UL1AnimNotifyState_OverlayEffect::NotifyTick(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComponent, Animation, FrameDeltaTime, EventReference);

	if (FOverlayEffectProgressInfo* ProgressInfo = ProgressInfoMap.Find(MeshComponent))
	{
		const float RateScale = bApplyRateScaleToProgress ? Animation->RateScale : 1.0f;
		ProgressInfo->ElapsedTime += FrameDeltaTime * RateScale;

		if (UMaterialInstanceDynamic* OverlayMaterialInstance = ProgressInfo->OverlayMaterialInstance)
		{
			const FLinearColor& Value = LinearColorCurve->GetLinearColorValue(ProgressInfo->ElapsedTime);
			OverlayMaterialInstance->SetVectorParameterValue(ParameterName, Value);
		}
	}
}

void UL1AnimNotifyState_OverlayEffect::NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (FOverlayEffectProgressInfo* ProgressInfo = ProgressInfoMap.Find(MeshComponent))
	{
		for (TWeakObjectPtr<UMeshComponent> CachedMeshComponent : ProgressInfo->MeshComponents)
		{
			if (CachedMeshComponent.IsValid())
			{
				CachedMeshComponent->SetOverlayMaterial(nullptr);
			}
		}
	}
	
	ProgressInfoMap.Remove(MeshComponent);
	
	Super::NotifyEnd(MeshComponent, Animation, EventReference);
}

void UL1AnimNotifyState_OverlayEffect::ApplyWeaponMeshComponent(FOverlayEffectProgressInfo& ProgressInfo, USkeletalMeshComponent* MeshComponent)
{
	if (WeaponHandType == EWeaponHandType::Count)
		return;
	
	if (ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(MeshComponent->GetOwner()))
	{
		if (UL1EquipManagerComponent* EquipManager = LyraCharacter->FindComponentByClass<UL1EquipManagerComponent>())
		{
			if (AL1EquipmentBase* WeaponActor = EquipManager->GetEquippedActor(WeaponHandType))
			{
				USkeletalMeshComponent* WeaponMeshComponent = WeaponActor->MeshComponent;
				WeaponMeshComponent->SetOverlayMaterial(ProgressInfo.OverlayMaterialInstance);
				ProgressInfo.MeshComponents.Add(WeaponMeshComponent);
			}
		}
	}
}

void UL1AnimNotifyState_OverlayEffect::ApplyAllWeaponMeshComponents(FOverlayEffectProgressInfo& ProgressInfo, USkeletalMeshComponent* MeshComponent)
{
	if (ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(MeshComponent->GetOwner()))
	{
		if (UL1EquipManagerComponent* EquipManager = LyraCharacter->FindComponentByClass<UL1EquipManagerComponent>())
		{
			TArray<AL1EquipmentBase*> WeaponActors;
			EquipManager->GetAllEquippedActors(WeaponActors);

			for (AL1EquipmentBase* WeaponActor : WeaponActors)
			{
				USkeletalMeshComponent* WeaponMeshComponent = WeaponActor->MeshComponent;
				WeaponMeshComponent->SetOverlayMaterial(ProgressInfo.OverlayMaterialInstance);
				ProgressInfo.MeshComponents.Add(WeaponMeshComponent);
			}
		}
	}
}

void UL1AnimNotifyState_OverlayEffect::ApplyCharacterMeshComponents(FOverlayEffectProgressInfo& ProgressInfo, USkeletalMeshComponent* MeshComponent)
{
	if (ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(MeshComponent->GetOwner()))
	{
		TArray<UMeshComponent*> CharacterMeshComponents;
		LyraCharacter->GetMeshComponents(CharacterMeshComponents);

		for (UMeshComponent* CharacterMeshComponent : CharacterMeshComponents)
		{
			CharacterMeshComponent->SetOverlayMaterial(ProgressInfo.OverlayMaterialInstance);
			ProgressInfo.MeshComponents.Add(CharacterMeshComponent);
		}
	}
}
