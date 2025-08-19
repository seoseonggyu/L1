﻿#pragma once

#include "AbilitySystem/Abilities/Legends/L1GameplayAbility_Equipment.h"
#include "L1GameplayAbility_Melee.generated.h"

class AL1EquipmentBase;

UCLASS()
class UL1GameplayAbility_Melee : public UL1GameplayAbility_Equipment
{
	GENERATED_BODY()
	
public:
	UL1GameplayAbility_Melee(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION()
	void ParseTargetData(const FGameplayAbilityTargetDataHandle& InTargetDataHandle, TArray<int32>& OutCharacterHitIndexes, TArray<int32>& OutBlockHitIndexes);

	UFUNCTION()
	void ProcessHitResult(FHitResult HitResult, float Damage, bool bBlockingHit, UAnimMontage* BackwardMontage, AL1EquipmentBase* WeaponActor);

	UFUNCTION()
	void ResetHitActors();
	
	UFUNCTION()
	void DrawDebugHitPoint(const FHitResult& HitResult);

public:
	bool IsCharacterBlockingHit(ALyraCharacter* TargetCharacter);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="L1|Melee")
	float BlockingAngle = 60.f;

	UPROPERTY(EditDefaultsOnly, Category="L1|Melee")
	float BlockHitDamageMultiplier = 0.3f;
	
	UPROPERTY(EditDefaultsOnly, Category="L1|Melee")
	bool bShowDebug = false;

protected:
	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> CachedHitActors;
};
