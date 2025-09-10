#include "L1GameplayAbility_Interact_Monster.h"

#include "CommonActivatableWidget.h"
#include "L1GameplayTags.h"
#include "Actions/AsyncAction_PushContentToLayerForPlayer.h"
#include "Character/LyraCharacter.h"
#include "Character/LyraHealthComponent.h"
#include "Item/Managers/L1InventoryManagerComponent.h"
#include "Item/Managers/L1EquipManagerComponent.h"
#include "Item/Managers/L1EquipmentManagerComponent.h"
#include "Item/Managers/L1ItemManagerComponent.h"
#include "Player/LyraPlayerController.h"
#include "UI/Item/Equipment/L1EquipmentSlotsWidget.h"
#include "UI/Item/Inventory/L1InventorySlotsWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1GameplayAbility_Interact_Monster)

UL1GameplayAbility_Interact_Monster::UL1GameplayAbility_Interact_Monster(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UL1GameplayAbility_Interact_Monster::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TriggerEventData == nullptr || bInitialized == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	ULyraHealthComponent* HealthComponent = InteractableActor->GetComponentByClass<ULyraHealthComponent>();
	if (HealthComponent == nullptr || HealthComponent->IsDeadOrDying() == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	
	if (HasAuthority(&CurrentActivationInfo))
	{
		UL1ItemManagerComponent* MyItemManager = GetLyraPlayerControllerFromActorInfo()->GetComponentByClass<UL1ItemManagerComponent>();
		UL1InventoryManagerComponent* OtherInventoryManager = InteractableActor->GetComponentByClass<UL1InventoryManagerComponent>();
		// MyItemManager->AddAllowedComponent(OtherInventoryManager); // SSG: 
	}

	if (IsLocallyControlled())
	{
		if (UAsyncAction_PushContentToLayerForPlayer* PushWidgetAction = UAsyncAction_PushContentToLayerForPlayer::PushContentToLayerForPlayer(GetLyraPlayerControllerFromActorInfo(), WidgetClass, WidgetLayerTag, true))
		{
			PushWidgetAction->AfterPush.AddDynamic(this, &ThisClass::OnAfterPushWidget);
			PushWidgetAction->Activate();
		}
	}
}

void UL1GameplayAbility_Interact_Monster::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (HasAuthority(&CurrentActivationInfo))
	{
		if (UL1ItemManagerComponent* MyItemManager = GetLyraPlayerControllerFromActorInfo()->GetComponentByClass<UL1ItemManagerComponent>())
		{
			UL1InventoryManagerComponent* OtherInventoryManager = InteractableActor->GetComponentByClass<UL1InventoryManagerComponent>();
			// MyItemManager->RemoveAllowedComponent(OtherInventoryManager);// SSG: 
		}
	}

	if (IsLocallyControlled() && PushedWidget)
	{
		PushedWidget->DeactivateWidget();
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UL1GameplayAbility_Interact_Monster::OnAfterPushWidget(UCommonActivatableWidget* InPushedWidget)
{
	PushedWidget = InPushedWidget;
	
	ALyraCharacter* LyraCharacter = GetLyraCharacterFromActorInfo();
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	
	FInventoryInitializeMessage MyInventoryInitMessage;
	MyInventoryInitMessage.InventoryManager = LyraCharacter->GetComponentByClass<UL1InventoryManagerComponent>();
	MessageSubsystem.BroadcastMessage(L1GameplayTags::Message_Initialize_MyInventory, MyInventoryInitMessage);
	
	FEquipmentInitializeMessage MyEquipmentInitMessage;
	MyEquipmentInitMessage.EquipmentManager = LyraCharacter->GetComponentByClass<UL1EquipmentManagerComponent>();
	MyEquipmentInitMessage.EquipManager = LyraCharacter->GetComponentByClass<UL1EquipManagerComponent>();
	MessageSubsystem.BroadcastMessage(L1GameplayTags::Message_Initialize_MyEquipment, MyEquipmentInitMessage);

	FInventoryInitializeMessage OtherInventoryInitMessage;
	OtherInventoryInitMessage.InventoryManager = InteractableActor->GetComponentByClass<UL1InventoryManagerComponent>();
	MessageSubsystem.BroadcastMessage(L1GameplayTags::Message_Initialize_OtherInventory, OtherInventoryInitMessage);

	InPushedWidget->OnDeactivated().AddWeakLambda(this, [this]()
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	});
}
