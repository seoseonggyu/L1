#include "L1GameplayAbility_Skill_CastRequest.h"

#include "Character/LyraCharacter.h"
#include "Network/L1NetworkManager.h"
#include "Network/NetworkUtils.h"
#include "Player/LyraPlayerState.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(L1GameplayAbility_Skill_CastRequest)

UL1GameplayAbility_Skill_CastRequest::UL1GameplayAbility_Skill_CastRequest(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UL1GameplayAbility_Skill_CastRequest::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(ActorInfo->AvatarActor.Get());
	if (LyraCharacter == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	LyraCharacter->GetPlayerState();
	
	// SSG: 스킬 입력 보내기
	if (UL1NetworkManager* NetworkManager = NetworkUtils::GetNetworkManager(Cast<ALyraPlayerState>(LyraCharacter->GetPlayerState())))
	{
		Protocol::C_SKILL_IMMEDIATE_CAST SkillImmediatePkt;
		SkillImmediatePkt.set_object_id(LyraCharacter->GetPlayerId());
		SkillImmediatePkt.set_skill_type(NetworkUtils::ConvertPortoFromGameplayTag(SendSkillTag));
		NetworkManager->SendPacket(SkillImmediatePkt);
	}
}