#include "L1ClassEntryWidget.h"

#include "L1ClassSelectionWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "L1ClassSkillEntryWidget.h"
#include "AbilitySystem/LyraAbilitySet.h"
#include "Components/Button.h"
#include "Data/L1ClassData.h"
#include "Player/LyraPlayerState.h"
#include "Network/L1NetworkManager.h"
#include "Network/NetworkUtils.h"
#include "Character/LyraCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1ClassEntryWidget)

UL1ClassEntryWidget::UL1ClassEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UL1ClassEntryWidget::InitializeUI(UL1ClassSelectionWidget* OwnerWidget, ECharacterClassType ClassType)
{
	CachedClassType = ClassType;
	CachedOwnerWidget = OwnerWidget;

	const FL1ClassInfoEntry& ClassEntry = UL1ClassData::Get().GetClassInfoEntry(ClassType);
	
	Text_ClassName->SetText(ClassEntry.ClassName);
	VerticalBox_SkillElements->ClearChildren();
	
	if (ULyraAbilitySet* AbilitySet = ClassEntry.ClassAbilitySet)
	{
		const TArray<FLyraAbilitySet_GameplayAbility>& AbilitySetAbilities = AbilitySet->GetGrantedGameplayAbilities();
		for (int i = 0; i < 2; i++)
		{
			if (AbilitySetAbilities.IsValidIndex(i))
			{
				const FLyraAbilitySet_GameplayAbility& AbilitySetAbility = AbilitySetAbilities[i];
				UL1ClassSkillEntryWidget* SkillEntryWidget = CreateWidget<UL1ClassSkillEntryWidget>(this, SkillEntryWidgetClass);
				SkillEntryWidget->InitializeUI(AbilitySetAbility.Ability);
				VerticalBox_SkillElements->AddChild(SkillEntryWidget);
			}
		}
	}

	Button_Class->OnClicked.AddUniqueDynamic(this, &ThisClass::OnButtonClicked);
}

void UL1ClassEntryWidget::OnButtonClicked()
{
	if (UL1NetworkManager* NetworkManager = NetworkUtils::GetNetworkManager(Cast<ALyraPlayerState>(GetOwningPlayerState())))
	{
		NetworkManager->SendPacket_SelectClass(CachedClassType, Cast<ALyraCharacter>(GetOwningPlayerPawn()));
	}

	if (UL1ClassSelectionWidget* ClassSelectionWidget = CachedOwnerWidget.Get())
	{
		ClassSelectionWidget->DeactivateWidget();
		ClassSelectionWidget = nullptr;
	}
}
