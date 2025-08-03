#include "L1CosmeticManagerComponent.h"

#include "L1Define.h"
#include "Actors/L1ArmorBase.h"
#include "Character/LyraCharacter.h"
#include "Data/L1CharacterData.h"
//#include "Item/Fragments/L1ItemFragment_Equipable_Armor.h"
#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1CosmeticManagerComponent)

UL1CosmeticManagerComponent::UL1CosmeticManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UL1CosmeticManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeManager();
}

void UL1CosmeticManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HeadSlot)
	{
		HeadSlot->DestroyComponent();
	}
	
	for (UChildActorComponent* CosmeticSlot : CosmeticSlots)
	{
		if (CosmeticSlot)
		{
			CosmeticSlot->DestroyComponent();
		}
	}
	
	Super::EndPlay(EndPlayReason);
}

void UL1CosmeticManagerComponent::RefreshArmorMesh(EArmorType ArmorType/*, const UL1ItemFragment_Equipable_Armor* ArmorFragment*/)
{
	if (ArmorType == EArmorType::Count)
		return;
	
	InitializeManager();
	
	/*if (ArmorFragment)
	{
		if (ArmorFragment == nullptr || ArmorFragment->ArmorType != ArmorType)
			return;
		
		SetPrimaryArmorMesh(ArmorType, ArmorFragment->ArmorMesh);
		
		if (ArmorType == EArmorType::Chest)
		{
			if (UChildActorComponent* CosmeticSlot = CosmeticSlots[(int32)EArmorType::Legs])
			{
				if (AL1ArmorBase* CosmeticActor = Cast<AL1ArmorBase>(CosmeticSlot->GetChildActor()))
				{
					CosmeticActor->ShouldUseSecondaryMesh(ArmorFragment->bIsFullBody);
				}
			}
		}
		else if (ArmorType == EArmorType::Helmet)
		{
			if (HeadSlot)
			{
				if (AL1ArmorBase* CosmeticActor = Cast<AL1ArmorBase>(HeadSlot->GetChildActor()))
				{
					CosmeticActor->ShouldUseSecondaryMesh(true);
				}
			}
		}
	}
	else*/
	{
		if (ArmorType == EArmorType::Chest)
		{
			if (UChildActorComponent* CosmeticSlot = CosmeticSlots[(int32)EArmorType::Legs])
			{
				if (AL1ArmorBase* CosmeticActor = Cast<AL1ArmorBase>(CosmeticSlot->GetChildActor()))
				{
					CosmeticActor->ShouldUseSecondaryMesh(false);
				}
			}
		}
		else if (ArmorType == EArmorType::Helmet)
		{
			if (HeadSlot)
			{
				if (AL1ArmorBase* CosmeticActor = Cast<AL1ArmorBase>(HeadSlot->GetChildActor()))
				{
					CosmeticActor->ShouldUseSecondaryMesh(false);
				}
			}
		}

		SetPrimaryArmorMesh(ArmorType, nullptr);
	}
}

void UL1CosmeticManagerComponent::SetPrimaryArmorMesh(EArmorType ArmorType, TSoftObjectPtr<USkeletalMesh> ArmorMeshPtr)
{
	if (ArmorType == EArmorType::Count)
		return;
	
	InitializeManager();
	
	if (UChildActorComponent* CosmeticSlot = CosmeticSlots[(int32)ArmorType])
	{
		if (AL1ArmorBase* CosmeticActor = Cast<AL1ArmorBase>(CosmeticSlot->GetChildActor()))
		{
			if (ArmorMeshPtr.IsNull())
			{
				CosmeticActor->SetPrimaryArmorMesh(nullptr);
			}
			else
			{
				USkeletalMesh* ArmorMesh = ULyraAssetManager::GetAssetByPath<USkeletalMesh>(ArmorMeshPtr);
				CosmeticActor->SetPrimaryArmorMesh(ArmorMesh);
			}
		}
	}
}

void UL1CosmeticManagerComponent::GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const
{
	if (HeadSlot)
	{
		if (AL1ArmorBase* CosmeticActor = Cast<AL1ArmorBase>(HeadSlot->GetChildActor()))
		{
			OutMeshComponents.Add(CosmeticActor->GetMeshComponent());
		}
	}
	
	for (UChildActorComponent* CosmeticSlot : CosmeticSlots)
	{
		if (CosmeticSlot)
		{
			if (AL1ArmorBase* CosmeticActor = Cast<AL1ArmorBase>(CosmeticSlot->GetChildActor()))
			{
				OutMeshComponents.Add(CosmeticActor->GetMeshComponent());
			}
		}
	}
}

void UL1CosmeticManagerComponent::InitializeManager()
{
	if (bInitialized)
		return;

	bInitialized = true;

	const int32 ArmorTypeCount = (int32)EArmorType::Count;
	CosmeticSlots.SetNumZeroed(ArmorTypeCount);
	
	check(CosmeticSlotClass);
	check(CharacterSkinType != ECharacterSkinType::Count);
	
	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		const UL1CharacterData& CharacterData = ULyraAssetManager::Get().GetCharacterData();
		const FL1DefaultArmorMeshSet& DefaultArmorMeshSet = CharacterData.GetDefaultArmorMeshSet(CharacterSkinType);
		
		HeadSlot = SpawnCosmeticSlotActor(DefaultArmorMeshSet.HeadDefaultMesh, DefaultArmorMeshSet.HeadSecondaryMesh, NAME_None, nullptr);
		
		for (int32 i = 0; i < (int32)EArmorType::Count; i++)
		{
			EArmorType ArmorType = (EArmorType)i;
			FName SkinMaterialSlotName;
			TSoftObjectPtr<UMaterialInterface> SkinMaterial;

			if (ArmorType == EArmorType::Helmet || ArmorType == EArmorType::Chest || ArmorType == EArmorType::Hands)
			{
				SkinMaterialSlotName = FName("UpperBody");
				//SkinMaterial = DefaultArmorMeshSet.UpperBodySkinMaterial;
			}
			else if (ArmorType == EArmorType::Legs || ArmorType == EArmorType::Foot)
			{
				SkinMaterialSlotName = FName("LowerBody");
				//SkinMaterial = DefaultArmorMeshSet.LowerBodySkinMaterial;
			}
			else if (ArmorType == EArmorType::FullBody)
			{
				SkinMaterialSlotName = FName("FullBody");
			}

			CosmeticSlots[i] = SpawnCosmeticSlotActor(DefaultArmorMeshSet.DefaultMeshEntries[i], DefaultArmorMeshSet.SecondaryMeshEntries[i], SkinMaterialSlotName, SkinMaterial);
		}
	}
}

UChildActorComponent* UL1CosmeticManagerComponent::SpawnCosmeticSlotActor(TSoftObjectPtr<USkeletalMesh> InDefaultMesh, TSoftObjectPtr<USkeletalMesh> InSecondaryMesh, FName InSkinMaterialSlotName, TSoftObjectPtr<UMaterialInterface> InSkinMaterial)
{
	UChildActorComponent* CosmeticComponent = nullptr;
	
	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		USceneComponent* ComponentToAttachTo = Character->GetMesh();
		CosmeticComponent = NewObject<UChildActorComponent>(Character);
		CosmeticComponent->SetupAttachment(ComponentToAttachTo);
		CosmeticComponent->SetChildActorClass(CosmeticSlotClass);
		CosmeticComponent->RegisterComponent();

		if (AL1ArmorBase* SpawnedActor = Cast<AL1ArmorBase>(CosmeticComponent->GetChildActor()))
		{
			if (USceneComponent* SpawnedRootComponent = SpawnedActor->GetRootComponent())
			{
				SpawnedRootComponent->AddTickPrerequisiteComponent(ComponentToAttachTo);
			}
			
			SpawnedActor->InitializeActor(InDefaultMesh, InSecondaryMesh, InSkinMaterialSlotName, InSkinMaterial);
		}
	}
	
	return CosmeticComponent;
}
