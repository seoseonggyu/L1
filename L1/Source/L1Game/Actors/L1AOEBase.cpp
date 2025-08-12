#include "L1AOEBase.h"

#include "Character/LyraCharacter.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Player/LyraPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1AOEBase)

AL1AOEBase::AL1AOEBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("ArrowComponent");
	SetRootComponent(ArrowComponent);

	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SphereComponent->SetupAttachment(ArrowComponent);
}

void AL1AOEBase::BeginPlay()
{
	Super::BeginPlay();

	AttackIntervalTime = AttackTotalTime / TargetAttackCount;
	if (AttackIntervalTime <= 0)
	{
		Destroy();
		return;
	}
	
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::StartAOE);
}

void AL1AOEBase::StartAOE()
{
	GetWorld()->GetTimerManager().SetTimer(AOETimerHandle, this, &ThisClass::TickAOE, AttackIntervalTime, true, StartDelay);
}

void AL1AOEBase::TickAOE()
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = GetOwner();
	SpawnParameters.Instigator = Cast<APawn>(GetOwner());
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FVector SphereLocation = SphereComponent->GetComponentLocation();
	float SphereRadius = SphereComponent->GetScaledSphereRadius();

	FVector RandDir = FMath::VRand();
	RandDir.Z = 0.f;
	RandDir = RandDir.GetSafeNormal();
	
	FVector SpawnLocation = SphereLocation + (RandDir * FMath::RandRange(0.f, SphereRadius));
	GetWorld()->SpawnActor<AActor>(AOEElementClass, SpawnLocation, FRotator::ZeroRotator, SpawnParameters);
	CurrentAttackCount++;
	
	if (CurrentAttackCount == TargetAttackCount)
	{
		GetWorld()->GetTimerManager().ClearTimer(AOETimerHandle);
		Destroy();
	}

	if (CameraShakeClass)
	{
		if (ALyraCharacter* LyraCharacter = Cast<ALyraCharacter>(GetOwner()))
		{
			if (ALyraPlayerController* LyraPlayerController = LyraCharacter->GetLyraPlayerController())
			{
				if (LyraPlayerController->IsLocalController())
				{
					LyraPlayerController->ClientStartCameraShake(CameraShakeClass);
				}
			}
		}
	}
}
