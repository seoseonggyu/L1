#include "L1GameplayAbility_Skill_Buff.h"

#include "L1GameplayTags.h"
#include "NiagaraSystem.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/LyraPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1GameplayAbility_Skill_Buff)

UL1GameplayAbility_Skill_Buff::UL1GameplayAbility_Skill_Buff(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bServerRespectsRemoteAbilityCancellation = false;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnlyTermination;
	
    AbilityTags.AddTag(L1GameplayTags::Ability_Attack_Skill_2);
	BlockAbilitiesWithTag.AddTag(L1GameplayTags::Ability_Jump);
	ActivationOwnedTags.AddTag(L1GameplayTags::Status_RejectHitReact);
	ActivationOwnedTags.AddTag(L1GameplayTags::Status_Skill);
}

void UL1GameplayAbility_Skill_Buff::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (K2_CheckAbilityCooldown() == false || K2_CheckAbilityCost() == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo();
	UCharacterMovementComponent* CharacterMovement = LyraCharacter->GetCharacterMovement();
	if (CharacterMovement->IsFalling())
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo) == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	SetCameraMode(CameraModeClass);

	if (ALyraPlayerController* LyraPlayerController = GetLyraPlayerControllerFromActorInfo())
	{
		LyraPlayerController->SetIgnoreMoveInput(true);
		LyraPlayerController->SetIgnoreLookInput(true);
	}

	if (UAbilityTask_PlayMontageAndWait* BuffMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("BuffMontage"), BuffMontage, 1.f, NAME_None, true))
	{
		BuffMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageFinished);
		BuffMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageFinished);
		BuffMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageFinished);
		BuffMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageFinished);
		BuffMontageTask->ReadyForActivation();
	}

	ApplyEffect();
	ApplyAdditionalEffects();
}

void UL1GameplayAbility_Skill_Buff::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ClearCameraMode();

	if (ALyraPlayerController* LyraPlayerController = GetLyraPlayerControllerFromActorInfo())
	{
		LyraPlayerController->SetIgnoreMoveInput(false);
		LyraPlayerController->SetIgnoreLookInput(false);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UL1GameplayAbility_Skill_Buff::ApplyEffect()
{
	if (BuffGameplayEffectClass == nullptr)
		return;

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (SourceASC == nullptr)
		return;
	
	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(BuffGameplayEffectClass);
	
	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(BuffEffect);
	EffectSpecHandle.Data->SetContext(EffectContextHandle);
	
	ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle);
}

void UL1GameplayAbility_Skill_Buff::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UL1GameplayAbility_Skill_Buff::ApplyAdditionalEffects_Implementation()
{
	
}
