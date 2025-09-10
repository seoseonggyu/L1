#include "L1WorldInteractable.h"

#include "L1GameplayTags.h"
#include "Character/LyraCharacter.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1WorldInteractable)

AL1WorldInteractable::AL1WorldInteractable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void AL1WorldInteractable::OnInteractActiveStarted(AActor* Interactor)
{
	if (IsValid(Interactor) == false)
		return;
	
	CachedInteractors.Add(Interactor);

	K2_OnInteractActiveStarted(Interactor);
}

void AL1WorldInteractable::OnInteractActiveEnded(AActor* Interactor)
{
	if (IsValid(Interactor) == false)
		return;
	
	CachedInteractors.Remove(Interactor);

	K2_OnInteractActiveEnded(Interactor);
}

void AL1WorldInteractable::OnInteractionSuccess(AActor* Interactor)
{
	if (IsValid(Interactor) == false)
		return;
	
	if (bCanUsed)
	{
		bIsUsed = true;
	}

	for (TWeakObjectPtr<AActor> CachedInteractor : CachedInteractors)
	{
		if (ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(CachedInteractor.Get()))
		{
			if (Interactor == LyraCharacter)
				continue;
			
			// SSG: AOE
			/*if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(LyraCharacter))
			{
				FGameplayTagContainer CancelAbilitiesTag;
				CancelAbilitiesTag.AddTag(L1GameplayTags::Ability_Interact_Active);
				ASC->CancelAbilities(&CancelAbilitiesTag);
			}*/
		}
	}
	
	CachedInteractors.Empty();
	
	K2_OnInteractionSuccess(Interactor);
}

bool AL1WorldInteractable::CanInteraction(const FL1InteractionQuery& InteractionQuery) const
{
	return bCanUsed ? (bIsUsed == false) : true;
}
