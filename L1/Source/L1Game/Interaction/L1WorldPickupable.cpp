#include "L1WorldPickupable.h"

#include "Data/L1ItemData.h"
#include "Item/L1ItemInstance.h"
#include "Item/L1ItemTemplate.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1WorldPickupable)

AL1WorldPickupable::AL1WorldPickupable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;
}

void AL1WorldPickupable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PickupInfo);
}

void AL1WorldPickupable::OnRep_PickupInfo()
{
	if (const UL1ItemInstance* ItemInstance = PickupInfo.PickupInstance.ItemInstance)
	{
		const UL1ItemTemplate& ItemTemplate = UL1ItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
		InteractionInfo.Content = ItemTemplate.DisplayName;
	}
	else if (TSubclassOf<UL1ItemTemplate> ItemTemplateClass = PickupInfo.PickupTemplate.ItemTemplateClass)
	{
		if (const UL1ItemTemplate* ItemTemplate = ItemTemplateClass->GetDefaultObject<UL1ItemTemplate>())
		{
			InteractionInfo.Content = ItemTemplate->DisplayName;
		}
	}
}

void AL1WorldPickupable::SetPickupInfo(const FL1PickupInfo& InPickupInfo)
{
	
	if (InPickupInfo.PickupInstance.ItemInstance || InPickupInfo.PickupTemplate.ItemTemplateClass)
	{
		PickupInfo = InPickupInfo;
		OnRep_PickupInfo();
	}
	else
	{
		Destroy();
	}
}
