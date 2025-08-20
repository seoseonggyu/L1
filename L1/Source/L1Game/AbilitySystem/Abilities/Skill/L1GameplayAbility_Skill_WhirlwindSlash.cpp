#include "L1GameplayAbility_Skill_WhirlwindSlash.h"

#include "L1GameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Character/LyraCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/LyraPlayerController.h"
#include "System/LyraGameData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1GameplayAbility_Skill_WhirlwindSlash)

UL1GameplayAbility_Skill_WhirlwindSlash::UL1GameplayAbility_Skill_WhirlwindSlash(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilityTags.AddTag(L1GameplayTags::Ability_Attack_Skill_1);
	BlockAbilitiesWithTag.AddTag(L1GameplayTags::Ability_Jump);
	ActivationOwnedTags.AddTag(L1GameplayTags::Status_RejectHitReact);
	ActivationOwnedTags.AddTag(L1GameplayTags::Status_Skill);
	
	FL1EquipmentInfo EquipmentInfo;
	EquipmentInfo.EquipmentType = EEquipmentType::Weapon;
	EquipmentInfo.WeaponHandType = EWeaponHandType::TwoHand;
	EquipmentInfo.RequiredWeaponType = EWeaponType::TwoHandSword;
	EquipmentInfos.Add(EquipmentInfo);

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = L1GameplayTags::GameplayEvent_Skill_1;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}

void UL1GameplayAbility_Skill_WhirlwindSlash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TriggerEventData == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo();
	if (LyraCharacter == nullptr || LyraCharacter->GetCharacterMovement()->IsFalling())
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

	// TODO: 애니메이션 걷는 속도랑, 마지막 도중 움직이는거 멈춰야함

	if (UAbilityTask_PlayMontageAndWait* WhirlwindSlashMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("WhirlwindSlashMontage"), WhirlwindSlashMontage, 1.f, NAME_None, true))
	{
		WhirlwindSlashMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageFinished);
		WhirlwindSlashMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageFinished);
		WhirlwindSlashMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageFinished);
		WhirlwindSlashMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageFinished);
		WhirlwindSlashMontageTask->ReadyForActivation();
	}

	if (UAbilityTask_WaitGameplayEvent* TraceEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, L1GameplayTags::GameplayEvent_Trace, nullptr, false, true))
	{
		TraceEventTask->EventReceived.AddDynamic(this, &ThisClass::OnTrace);
		TraceEventTask->ReadyForActivation();
	}

	if (UAbilityTask_WaitGameplayEvent* ResetEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, L1GameplayTags::GameplayEvent_Reset, nullptr, false, true))
	{
		ResetEventTask->EventReceived.AddDynamic(this, &ThisClass::OnReset);
		ResetEventTask->ReadyForActivation();
	}

	if (UAbilityTask_WaitGameplayEvent* WhirlwindSlashBeginEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, L1GameplayTags::GameplayEvent_Montage_Begin, nullptr, true, true))
	{
		WhirlwindSlashBeginEventTask->EventReceived.AddDynamic(this, &ThisClass::OnWhirlwindSlashBegin);
		WhirlwindSlashBeginEventTask->ReadyForActivation();
	}

	if (UAbilityTask_WaitGameplayEvent* WhirlwindSlashEndEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, L1GameplayTags::GameplayEvent_Montage_End, nullptr, true, true))
	{
		WhirlwindSlashEndEventTask->EventReceived.AddDynamic(this, &ThisClass::OnWhirlwindSlashEnd);
		WhirlwindSlashEndEventTask->ReadyForActivation();
	}
}

void UL1GameplayAbility_Skill_WhirlwindSlash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ClearCameraMode();

	// TODO: 여기서 스킬 리셋
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetLyraCharacterFromActorInfo(), L1GameplayTags::GameplayEvent_Reset_Skill_1, FGameplayEventData());
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UL1GameplayAbility_Skill_WhirlwindSlash::OnTrace(FGameplayEventData Payload)
{
	ULyraAbilitySystemComponent* SourceASC = GetLyraAbilitySystemComponentFromActorInfo();
	if (SourceASC == nullptr)
		return;

	AL1EquipmentBase* EquipmentActor = GetFirstEquipmentActor();
	check(EquipmentActor);

	if (SourceASC->FindAbilitySpecFromHandle(CurrentSpecHandle))
	{
		FGameplayAbilityTargetDataHandle LocalTargetDataHandle(MoveTemp(const_cast<FGameplayAbilityTargetDataHandle&>(Payload.TargetData)));

		TArray<int32> CharacterHitIndexes;
		TArray<int32> BlockHitIndexes;
		ParseTargetData(LocalTargetDataHandle, CharacterHitIndexes, BlockHitIndexes);

		for (int32 BlockHitIndex : BlockHitIndexes)
		{
			FHitResult HitResult = *(LocalTargetDataHandle.Data[BlockHitIndex]->GetHitResult());
			ProcessHitResult(HitResult, Damage, true, nullptr, EquipmentActor);
		}

		for (int32 CharacterHitIndex : CharacterHitIndexes)
		{
			FHitResult HitResult = *LocalTargetDataHandle.Data[CharacterHitIndex]->GetHitResult();
			ProcessHitResult(HitResult, Damage, false, nullptr, EquipmentActor);
		}
	}
}

void UL1GameplayAbility_Skill_WhirlwindSlash::OnReset(FGameplayEventData Payload)
{
	ResetHitActors();
}

void UL1GameplayAbility_Skill_WhirlwindSlash::OnWhirlwindSlashBegin(FGameplayEventData Payload)
{

}

void UL1GameplayAbility_Skill_WhirlwindSlash::OnWhirlwindSlashEnd(FGameplayEventData Payload)
{
}

void UL1GameplayAbility_Skill_WhirlwindSlash::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
