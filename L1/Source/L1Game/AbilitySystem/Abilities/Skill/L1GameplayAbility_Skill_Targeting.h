#pragma once

#include "AbilitySystem/Abilities/Legends/L1GameplayAbility_Equipment.h"
#include "L1GameplayAbility_Skill_Targeting.generated.h"

class AL1GameplayAbilityTargetActor_LineTraceHighlight;
class AGameplayAbilityWorldReticle;
class UAbilityTask_WaitConfirmCancel;
class UAbilityTask_WaitGameplayEvent;

UCLASS()
class UL1GameplayAbility_Skill_Targeting : public UL1GameplayAbility_Equipment
{
	GENERATED_BODY()
	
public:
	UL1GameplayAbility_Skill_Targeting(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UFUNCTION(BlueprintCallable)
	void ConfirmSkill();

	UFUNCTION(BlueprintCallable)
	void CancelSkill();

	UFUNCTION(BlueprintCallable)
	void ResetSkill();
	
	UFUNCTION(BlueprintImplementableEvent)
	void WaitTargetData();
	
private:
	UFUNCTION()
	void OnCastStartBegin(FGameplayEventData Payload);

	UFUNCTION()
	void OnSpellBegin(FGameplayEventData Payload);
	
	UFUNCTION()
	void OnMontageFinished();

private:
	UFUNCTION()
	void OnInputConfirm();
	
	UFUNCTION()
	void OnInputCancel();

protected:
	UPROPERTY(EditDefaultsOnly, Category="L1|Targeting")
	UAnimMontage* CastStartMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="L1|Targeting")
	UAnimMontage* CastEndMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="L1|Targeting")
	UAnimMontage* SpellMontage = nullptr;

protected:
	UPROPERTY(EditDefaultsOnly, Category="L1|Targeting", meta=(Categories="GameplayCue"))
	FGameplayTag CastGameplayCueTag;

	UPROPERTY(EditDefaultsOnly, Category="L1|Targeting", meta=(Categories="GameplayCue"))
	FGameplayTag BurstGameplayCueTag;

	UPROPERTY(EditDefaultsOnly, Category="L1|Targeting")
	TArray<TSubclassOf<UGameplayEffect>> GameplayEffectClasses;
	
protected:

	UPROPERTY(EditDefaultsOnly, Category="L1|Targeting")
	TObjectPtr<UInputAction> MainHandInputAction;

	UPROPERTY(EditDefaultsOnly, Category="L1|Targeting")
	TObjectPtr<UInputAction> OffHandInputAction;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="L1|Targeting")
	TSubclassOf<AL1GameplayAbilityTargetActor_LineTraceHighlight> TargetActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="L1|Targeting", DisplayName="Targeting Reticle Class")
	TSubclassOf<AGameplayAbilityWorldReticle> TargetingReticleClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="L1|Targeting")
	float MaxRange = 1000.f;

private:
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> CastStartBeginEventTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitConfirmCancel> SkillConfirmCancelTask;
	
	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	FGameplayAbilityTargetDataHandle TargetDataHandle;
};
