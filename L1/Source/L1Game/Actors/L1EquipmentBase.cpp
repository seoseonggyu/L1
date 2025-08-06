#include "L1EquipmentBase.h"

#include "Character/LyraCharacter.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Data/L1ItemData.h"
#include "Item/Fragments/L1ItemFragment_Equipable_Weapon.h"
#include "Kismet/KismetMathLibrary.h"
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
	EquipmentSlot();
}

void AL1EquipmentBase::Destroyed()
{
	if (ALyraCharacter* Character = Cast<ALyraCharacter>(GetOwner()))
	{
		if (ULyraAbilitySystemComponent* ASC = Cast<ULyraAbilitySystemComponent>(Character->GetAbilitySystemComponent()))
		{
			SkillAbilitySetHandles.TakeFromAbilitySystem(ASC);
		}

		// SSG: 
		/*if (UL1EquipManagerComponent* EquipManager = Character->FindComponentByClass<UL1EquipManagerComponent>())
		{
			TArray<FL1EquipEntry>& Entries = EquipManager->GetAllEntries();
			if (Entries[(int32)EquipmentSlotType].GetEquipmentActor() == this)
			{
				Entries[(int32)EquipmentSlotType].SetEquipmentActor(nullptr);
			}
		}*/
	}

	Super::Destroyed();
}

void AL1EquipmentBase::Init(int32 InTemplateID, EEquipmentSlotType InEquipmentSlotType)
{
	TemplateID = InTemplateID;
	EquipmentSlotType = InEquipmentSlotType;
}

void AL1EquipmentBase::ChangeBlockState(bool bShouldBlock)
{
	bCanBlock = bShouldBlock;
	CanBlock();
}

void AL1EquipmentBase::EquipmentSlot()
{
	// SSG: 
	/*if (GetOwner() && GetOwner()->FindComponentByClass<UL1EquipManagerComponent>())
	{
		if (ALyraCharacter* Character = Cast<ALyraCharacter>(GetOwner()))
		{
			if (UL1EquipManagerComponent* EquipManager = Character->FindComponentByClass<UL1EquipManagerComponent>())
			{
				TArray<FL1EquipEntry>& Entries = EquipManager->GetAllEntries();
				Entries[(int32)EquipmentSlotType].SetEquipmentActor(this);
			}
		}
	}
	else
	{
		GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::EquipmentSlot);
	}*/
}

void AL1EquipmentBase::CanBlock()
{
	// SSG: 
	// MeshComponent->SetCollisionResponseToChannel(L1_ObjectChannel_Projectile, bCanBlock ? ECR_Block : ECR_Ignore);
}

UAbilitySystemComponent* AL1EquipmentBase::GetAbilitySystemComponent() const
{
	UAbilitySystemComponent* ASC = nullptr;
	if (ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(GetOwner()))
	{
		ASC = LyraCharacter->GetAbilitySystemComponent();
	}
	return ASC;
}

UAnimMontage* AL1EquipmentBase::GetEquipMontage()
{
	UAnimMontage* EquipMontage = nullptr;

	if (TemplateID > 0)
	{
		const UL1ItemTemplate& ItemTemplate = UL1ItemData::Get().FindItemTemplateByID(TemplateID);
		if (const UL1ItemFragment_Equipable_Attachment* AttachmentFragment = ItemTemplate.FindFragmentByClass<UL1ItemFragment_Equipable_Attachment>())
		{
			EquipMontage = ULyraAssetManager::GetAssetByPath<UAnimMontage>(AttachmentFragment->EquipMontage);
		}
	}

	return EquipMontage;
}

UAnimMontage* AL1EquipmentBase::GetHitMontage(AActor* InstigatorActor, const FVector& HitLocation, bool IsBlocked)
{
	UAnimMontage* SelectedMontage = nullptr;

	if (InstigatorActor && TemplateID > 0)
	{
		const UL1ItemTemplate& ItemTemplate = UL1ItemData::Get().FindItemTemplateByID(TemplateID);
		if (const UL1ItemFragment_Equipable_Attachment* AttachmentFragment = ItemTemplate.FindFragmentByClass<UL1ItemFragment_Equipable_Attachment>())
		{
			if (IsBlocked)
			{
				SelectedMontage = ULyraAssetManager::GetAssetByPath<UAnimMontage>(AttachmentFragment->BlockHitMontage);
			}
			else
			{
				AActor* CharacterActor = GetOwner();
				const FVector& CharacterLocation = CharacterActor->GetActorLocation();
				const FVector& CharacterDirection = CharacterActor->GetActorForwardVector();

				const FRotator& FacingRotator = UKismetMathLibrary::Conv_VectorToRotator(CharacterDirection);
				const FRotator& CharacterToHitRotator = UKismetMathLibrary::Conv_VectorToRotator((HitLocation - CharacterLocation).GetSafeNormal());

				const FRotator& DeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(CharacterToHitRotator, FacingRotator);
				float YawAbs = FMath::Abs(DeltaRotator.Yaw);

				if (YawAbs < 60.f)
				{
					SelectedMontage = ULyraAssetManager::GetAssetByPath<UAnimMontage>(AttachmentFragment->FrontHitMontage);
				}
				else if (YawAbs > 120.f)
				{
					SelectedMontage = ULyraAssetManager::GetAssetByPath<UAnimMontage>(AttachmentFragment->BackHitMontage);
				}
				else if (DeltaRotator.Yaw < 0.f)
				{
					SelectedMontage = ULyraAssetManager::GetAssetByPath<UAnimMontage>(AttachmentFragment->LeftHitMontage);
				}
				else
				{
					SelectedMontage = ULyraAssetManager::GetAssetByPath<UAnimMontage>(AttachmentFragment->RightHitMontage);
				}
			}
		}
	}

	return SelectedMontage;
}
