#include "L1GameplayAbility_Skill_AOE.h"

#include "L1GameplayTags.h"
#include "GameplayCueFunctionLibrary.h"
#include "Abilities/GameplayAbilityTargetActor_GroundTrace.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitConfirmCancel.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Actors/L1AOEBase.h" 
#include "Actors/L1EquipmentBase.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "UI/HUD/L1SkillInputWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1GameplayAbility_Skill_AOE)

UL1GameplayAbility_Skill_AOE::UL1GameplayAbility_Skill_AOE(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    AbilityTags.AddTag(L1GameplayTags::Ability_Attack_Skill_1);
	ActivationOwnedTags.AddTag(L1GameplayTags::Status_Skill);

	FL1EquipmentInfo EquipmentInfo;
	EquipmentInfo.WeaponHandType = EWeaponHandType::TwoHand;
	EquipmentInfo.RequiredWeaponType = EWeaponType::Staff;
	EquipmentInfos.Add(EquipmentInfo);
}

void UL1GameplayAbility_Skill_AOE::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData); 
	
	if (K2_CheckAbilityCooldown() == false || K2_CheckAbilityCost() == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	
	if (UAbilityTask_PlayMontageAndWait* CastStartMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("CastStartMontage"), CastStartMontage, 1.f, NAME_None, true))
	{
		CastStartMontageTask->ReadyForActivation();
	}

	CastStartBeginEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, L1GameplayTags::GameplayEvent_Montage_Begin, nullptr, true, true);
	if (CastStartBeginEventTask)
	{
		CastStartBeginEventTask->EventReceived.AddDynamic(this, &ThisClass::OnCastStartBegin);
		CastStartBeginEventTask->ReadyForActivation();
	}

	SkillConfirmCancelTask = UAbilityTask_WaitConfirmCancel::WaitConfirmCancel(this);
	if (SkillConfirmCancelTask)
	{
		SkillConfirmCancelTask->OnConfirm.AddDynamic(this, &ThisClass::OnInputConfirm);
		SkillConfirmCancelTask->OnCancel.AddDynamic(this, &ThisClass::OnInputCancel);
		SkillConfirmCancelTask->ReadyForActivation();
	}
}

void UL1GameplayAbility_Skill_AOE::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ResetSkill();
	
	FGameplayCueParameters Parameters;
	UGameplayCueFunctionLibrary::RemoveGameplayCueOnActor(GetAvatarActorFromActorInfo(), CastGameplayCueTag, Parameters);
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	FlushPressedInput(MainHandInputAction);
	FlushPressedInput(OffHandInputAction);
}

void UL1GameplayAbility_Skill_AOE::ConfirmSkill()
{
	ResetSkill();

	if (UAbilityTask_PlayMontageAndWait* SpellMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("SpellMontage"), SpellMontage, 1.f, NAME_None, false))
	{
		SpellMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageFinished);
		SpellMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageFinished);
		SpellMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageFinished);
		SpellMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageFinished);
		SpellMontageTask->ReadyForActivation();
	}

	if (UAbilityTask_WaitGameplayEvent* SpellBeginEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, L1GameplayTags::GameplayEvent_Montage_Begin, nullptr, true, true))
	{
		SpellBeginEventTask->EventReceived.AddDynamic(this, &ThisClass::OnSpellBegin);
		SpellBeginEventTask->ReadyForActivation();
	}

	if (UAbilityTask_WaitGameplayEvent* SpellEndEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, L1GameplayTags::GameplayEvent_Montage_End, nullptr, true, true))
	{
		SpellEndEventTask->EventReceived.AddDynamic(this, &ThisClass::OnSpellEnd);
		SpellEndEventTask->ReadyForActivation();
	}
}

void UL1GameplayAbility_Skill_AOE::CancelSkill()
{
	ResetSkill();
	
	FGameplayCueParameters Parameters;
	UGameplayCueFunctionLibrary::RemoveGameplayCueOnActor(GetAvatarActorFromActorInfo(), CastGameplayCueTag, Parameters);

	if (UAbilityTask_PlayMontageAndWait* CastEndMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("CastEndMontage"), CastEndMontage, 1.f, NAME_None, false))
	{
		CastEndMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageFinished);
		CastEndMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageFinished);
		CastEndMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageFinished);
		CastEndMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageFinished);
		CastEndMontageTask->ReadyForActivation();
	}
}

void UL1GameplayAbility_Skill_AOE::ResetSkill()
{
	if (UGameplayMessageSubsystem::HasInstance(this))
	{
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);

		FSkillInputInitializeMessage Message;
		Message.bShouldShow = false;
		MessageSubsystem.BroadcastMessage(L1GameplayTags::Message_HUD_Spell_Input, Message);
	}

	if (CastStartBeginEventTask)
	{
		CastStartBeginEventTask->EndTask();
	}

	if (SkillConfirmCancelTask)
	{
		SkillConfirmCancelTask->EndTask();
	}
}

void UL1GameplayAbility_Skill_AOE::OnCastStartBegin(FGameplayEventData Payload)
{
	FGameplayCueParameters Parameters;
	Parameters.EffectCauser = GetFirstEquipmentActor();
	UGameplayCueFunctionLibrary::AddGameplayCueOnActor(GetAvatarActorFromActorInfo(), CastGameplayCueTag, Parameters);

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	
	FSkillInputInitializeMessage Message;
	Message.bShouldShow = true;
	MessageSubsystem.BroadcastMessage(L1GameplayTags::Message_HUD_Spell_Input, Message);

#if 1
	WaitTargetData();
#else
	AGameplayAbilityTargetActor_GroundTrace* TargetActor = GetWorld()->SpawnActorDeferred<AGameplayAbilityTargetActor_GroundTrace>(TargetActorClass, FTransform::Identity, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	TargetActor->CollisionRadius = CollisionRadius;
	TargetActor->CollisionHeight = CollisionHeight;
	TargetActor->MaxRange = MaxRange;
	TargetActor->TraceProfile = TraceProfile;
	TargetActor->StartLocation = MakeTargetLocationInfoFromOwnerSkeletalMeshComponent("head");
	TargetActor->ReticleClass = AOEReticleClass;
	TargetActor->FinishSpawning(FTransform::Identity);
	
	if (UAbilityTask_WaitTargetData* TargetDataTask = UAbilityTask_WaitTargetData::WaitTargetDataUsingActor(this, TEXT("TargetDataTask"), EGameplayTargetingConfirmation::UserConfirmed, TargetActor))
	{
		TargetDataTask->ValidData.AddDynamic(this, &ThisClass::OnValidData);
		TargetDataTask->Cancelled.AddDynamic(this, &ThisClass::OnCancelled);
		TargetDataTask->ReadyForActivation();
	}
#endif
}

void UL1GameplayAbility_Skill_AOE::OnSpellBegin(FGameplayEventData Payload)
{
	if (TargetDataHandle.Data.IsValidIndex(0))
	{
		if (FGameplayAbilityTargetData* TargetData = TargetDataHandle.Data[0].Get())
		{
			if (const FHitResult* HitResultPtr = TargetData->GetHitResult())
			{
				const FHitResult& HitResult = *HitResultPtr;
				const FVector HitLocation = HitResult.Location;
				const FVector StartLocation = GetActorInfo().SkeletalMeshComponent->GetSocketLocation("head");
				
				if (FVector::DistSquared(HitLocation, StartLocation) > FMath::Square(MaxRange * AcceptanceMultiplier))
					return;

				FTransform SpawnTransform = FTransform::Identity;
				SpawnTransform.SetLocation(HitResult.Location);
				
				AL1AOEBase* AOE = GetWorld()->SpawnActorDeferred<AL1AOEBase>(
					AOESpawnerClass,
					SpawnTransform,
					GetAvatarActorFromActorInfo(),
					Cast<APawn>(GetAvatarActorFromActorInfo()),
					ESpawnActorCollisionHandlingMethod::AlwaysSpawn
				);
	
				AOE->FinishSpawning(SpawnTransform);
			}
		}
	}

	TargetDataHandle.Clear();
}

void UL1GameplayAbility_Skill_AOE::OnSpellEnd(FGameplayEventData Payload)
{
	FGameplayCueParameters Parameters;
	UGameplayCueFunctionLibrary::RemoveGameplayCueOnActor(GetAvatarActorFromActorInfo(), CastGameplayCueTag, Parameters);
}

void UL1GameplayAbility_Skill_AOE::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UL1GameplayAbility_Skill_AOE::OnInputConfirm()
{
	ConfirmTaskByInstanceName(TEXT("TargetDataTask"), true);
}

void UL1GameplayAbility_Skill_AOE::OnInputCancel()
{
	CancelSkill();
}

void UL1GameplayAbility_Skill_AOE::OnValidTargetData(const FGameplayAbilityTargetDataHandle& Data)
{
	TargetDataHandle = Data;

	bool bCommitted = CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
	bCommitted ? ConfirmSkill() : CancelSkill();
}

void UL1GameplayAbility_Skill_AOE::OnTargetCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
	CancelSkill();
}