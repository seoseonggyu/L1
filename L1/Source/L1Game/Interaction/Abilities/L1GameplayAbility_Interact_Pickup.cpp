#include "L1GameplayAbility_Interact_Pickup.h"

#include "Actors/L1PickupableItemBase.h"
#include "Item/Managers/L1ItemManagerComponent.h"
#include "Player/LyraPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1GameplayAbility_Interact_Pickup)

UL1GameplayAbility_Interact_Pickup::UL1GameplayAbility_Interact_Pickup(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    bServerRespectsRemoteAbilityCancellation = false;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnlyTermination;
}

void UL1GameplayAbility_Interact_Pickup::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TriggerEventData == nullptr || bInitialized == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (HasAuthority(&CurrentActivationInfo) == false)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;	
	}

	UL1ItemManagerComponent* ItemManager = GetLyraPlayerControllerFromActorInfo()->GetComponentByClass<UL1ItemManagerComponent>();
	if (ItemManager == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	
	AL1PickupableItemBase* PickupableActor = Cast<AL1PickupableItemBase>(InteractableActor);
	if (PickupableActor == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	// SSG: 
	/*if (ItemManager->TryPickItem(PickupableActor))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
	else
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}*/
}
