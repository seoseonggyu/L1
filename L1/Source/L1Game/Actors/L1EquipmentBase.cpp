#include "L1EquipmentBase.h"

#include "AbilitySystemComponent.h"
#include "L1GameplayTags.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Character/LyraCharacter.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Data/L1ItemData.h"
#include "Item/L1ItemInstance.h"
#include "Item/Fragments/L1ItemFragment_Equipable_Weapon.h"
#include "Item/Managers/L1EquipManagerComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Physics/LyraCollisionChannels.h"
#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1EquipmentBase)

AL1EquipmentBase::AL1EquipmentBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("ArrowComponent");
	ArrowComponent->PrimaryComponentTick.bStartWithTickEnabled = false;
	SetRootComponent(ArrowComponent);

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	MeshComponent->SetCollisionProfileName("Weapon");
	MeshComponent->SetGenerateOverlapEvents(false);
	MeshComponent->SetupAttachment(GetRootComponent());
	MeshComponent->PrimaryComponentTick.bStartWithTickEnabled = false;
	MeshComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;

	TraceDebugCollision = CreateDefaultSubobject<UBoxComponent>("TraceDebugCollision");
	TraceDebugCollision->SetCollisionProfileName("NoCollision");
	TraceDebugCollision->SetGenerateOverlapEvents(false);
	TraceDebugCollision->SetupAttachment(GetRootComponent());
	TraceDebugCollision->PrimaryComponentTick.bStartWithTickEnabled = false;
}

void AL1EquipmentBase::BeginPlay()
{
	Super::BeginPlay();
	ProcessEquipmentSlotType();

}

void AL1EquipmentBase::Destroyed()
{
	if (ALyraCharacter* Character = Cast<ALyraCharacter>(GetOwner()))
	{
		if (ULyraAbilitySystemComponent* ASC = Cast<ULyraAbilitySystemComponent>(Character->GetAbilitySystemComponent()))
		{
			SkillAbilitySetHandles.TakeFromAbilitySystem(ASC);
		}

		if (UL1EquipManagerComponent* EquipManager = Character->FindComponentByClass<UL1EquipManagerComponent>())
		{
			TArray<FL1EquipEntry>& Entries = EquipManager->GetEntries();
			if (Entries[(int32)EquipmentSlotType].GetEquipmentActor() == this)
			{
				Entries[(int32)EquipmentSlotType].SetEquipmentActor(nullptr);
			}
		}
	}

	Super::Destroyed();
}

void AL1EquipmentBase::Init(int32 InTemplateID, EEquipmentSlotType InEquipmentSlotType)
{
	ItemTemplateID = InTemplateID;
	EquipmentSlotType = InEquipmentSlotType;
}

void AL1EquipmentBase::ChangeBlockState(bool bShouldBlock)
{
	bCanBlock = bShouldBlock;
	ProcessCanBlock();
}

void AL1EquipmentBase::ProcessCanBlock()
{
	MeshComponent->SetCollisionResponseToChannel(L1_ObjectChannel_Projectile, bCanBlock ? ECR_Block : ECR_Ignore);
}

void AL1EquipmentBase::ProcessItemTemplateID()
{
	CheckPropertyInitialization();
}

void AL1EquipmentBase::ProcessEquipmentSlotType()
{
	CheckPropertyInitialization();
}

UAbilitySystemComponent* AL1EquipmentBase::GetAbilitySystemComponent() const
{
	if (ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(GetOwner()))
	{
		return LyraCharacter->GetAbilitySystemComponent();
	}
	return nullptr;
}

void AL1EquipmentBase::ProcessEquip_Implementation(UL1ItemInstance* ItemInstance)
{
	ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(GetOwner());
	if (LyraCharacter == nullptr)
		return;

	UAbilitySystemComponent* ASC = LyraCharacter->GetAbilitySystemComponent();
	if (ASC == nullptr)
		return;

	check(ItemTemplateID > 0);

	const UL1ItemTemplate& ItemTemplate = UL1ItemData::Get().FindItemTemplateByID(ItemTemplateID);
	const UL1ItemFragment_Equipable_Attachment* AttachmentFragment = ItemTemplate.FindFragmentByClass<UL1ItemFragment_Equipable_Attachment>();
	if (AttachmentFragment == nullptr)
		return;

	if (USkeletalMeshComponent* CharacterMeshComponent = LyraCharacter->GetMesh())
	{
		if (AttachmentFragment->AnimInstanceClass)
		{
			CharacterMeshComponent->LinkAnimClassLayers(AttachmentFragment->AnimInstanceClass);
		}

		if (ASC->HasMatchingGameplayTag(L1GameplayTags::Status_Interact) == false)
		{
			PlayEquipMontage();
		}
	}
}

void AL1EquipmentBase::PlayEquipMontage()
{
	ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(GetOwner());
	if (LyraCharacter == nullptr)
		return;

	USkeletalMeshComponent* CharacterMeshComponent = LyraCharacter->GetMesh();
	if (CharacterMeshComponent == nullptr)
		return;

	UAnimMontage* EquipMontage = ULyraAssetManager::GetAssetByPath<UAnimMontage>(GetEquipMontage());
	if (UAnimInstance* AnimInstance = CharacterMeshComponent->GetAnimInstance())
	{
		if (AnimInstance->GetCurrentActiveMontage() != EquipMontage)
		{
			LyraCharacter->PlayAnimMontage(EquipMontage);
		}
	}
}

void AL1EquipmentBase::CheckPropertyInitialization()
{
	if (bInitialized)
		return;

	if (ItemTemplateID <= 0 || EquipmentSlotType == EEquipmentSlotType::Count)
		return;

	if (GetOwner())
	{
		UL1EquipManagerComponent* EquipManager = GetOwner()->FindComponentByClass<UL1EquipManagerComponent>();
		UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
		if (EquipManager && ASC)
		{
			if (UL1ItemInstance* ItemInstance = EquipManager->GetEquippedItemInstance(EquipmentSlotType))
			{
				TArray<FL1EquipEntry>& Entries = EquipManager->GetEntries();
				Entries[(int32)EquipmentSlotType].SetEquipmentActor(this);

				ProcessEquip(ItemInstance);
				bInitialized = true;
				return;
			}
		}
	}
	GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::CheckPropertyInitialization);
}

TSoftObjectPtr<UAnimMontage> AL1EquipmentBase::GetEquipMontage() const
{
	if (ItemTemplateID <= 0)
		return nullptr;

	const UL1ItemTemplate& ItemTemplate = UL1ItemData::Get().FindItemTemplateByID(ItemTemplateID);
	const UL1ItemFragment_Equipable_Attachment* AttachmentFragment = ItemTemplate.FindFragmentByClass<UL1ItemFragment_Equipable_Attachment>();
	if (AttachmentFragment == nullptr)
		return nullptr;

	return AttachmentFragment->EquipMontage;
}

UAnimMontage* AL1EquipmentBase::GetHitMontage(AActor* InstigatorActor, const FVector& HitLocation, bool IsBlocked)
{
	UAnimMontage* SelectedMontage = nullptr;

	if (InstigatorActor && ItemTemplateID > 0)
	{
		const UL1ItemTemplate& ItemTemplate = UL1ItemData::Get().FindItemTemplateByID(ItemTemplateID);
		if (const UL1ItemFragment_Equipable_Attachment* AttachmentFragment = ItemTemplate.FindFragmentByClass<UL1ItemFragment_Equipable_Attachment>())
		{
			TSoftObjectPtr<UAnimMontage> SelectedMontagePath = nullptr;

			if (IsBlocked)
			{
				SelectedMontagePath = AttachmentFragment->BlockHitMontage;
			}
			else
			{
				AActor* CharacterActor = GetOwner();
				FVector CharacterLocation = CharacterActor->GetActorLocation();
				FVector CharacterDirection = CharacterActor->GetActorForwardVector();

				FRotator FacingRotator = UKismetMathLibrary::Conv_VectorToRotator(CharacterDirection);
				FRotator CharacterToHitRotator = UKismetMathLibrary::Conv_VectorToRotator((HitLocation - CharacterLocation).GetSafeNormal());

				FRotator DeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(CharacterToHitRotator, FacingRotator);
				float YawAbs = FMath::Abs(DeltaRotator.Yaw);

				if (YawAbs < 60.f)
				{
					SelectedMontagePath = AttachmentFragment->FrontHitMontage;
				}
				else if (YawAbs > 120.f)
				{
					SelectedMontagePath = AttachmentFragment->BackHitMontage;
				}
				else if (DeltaRotator.Yaw < 0.f)
				{
					SelectedMontagePath = AttachmentFragment->LeftHitMontage;
				}
				else
				{
					SelectedMontagePath = AttachmentFragment->RightHitMontage;
				}
			}

			if (SelectedMontagePath.IsNull() == false)
			{
				SelectedMontage = ULyraAssetManager::GetAssetByPath<UAnimMontage>(SelectedMontagePath);
			}
		}
	}

	return SelectedMontage;
}
