//#include "L1TargetPointBase.h"
//
//#include "GameModes/LyraExperienceManagerComponent.h"
//#include "Kismet/GameplayStatics.h"
//#include "System/L1ActorSpawningManagerComponent.h"
//
//#include UE_INLINE_GENERATED_CPP_BY_NAME(L1TargetPointBase)
//
//AL1TargetPointBase::AL1TargetPointBase(const FObjectInitializer& ObjectInitializer)
//	: Super(ObjectInitializer)
//{
//	PrimaryActorTick.bCanEverTick = false;
//	PrimaryActorTick.bStartWithTickEnabled = false;
//	
//	PreviewMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewMeshComponent"));
//	PreviewMeshComponent->SetupAttachment(RootComponent);
//	PreviewMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
//	PreviewMeshComponent->SetHiddenInGame(true);
//	PreviewMeshComponent->bIsEditorOnly = true;
//}
//
//void AL1TargetPointBase::BeginPlay()
//{
//	Super::BeginPlay();
//
//	if (HasAuthority() == false)
//		return;
//
//	AGameStateBase* GameState = UGameplayStatics::GetGameState(GetWorld());
//	ULyraExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<ULyraExperienceManagerComponent>();
//	check(ExperienceComponent);
//	ExperienceComponent->CallOrRegister_OnExperienceLoaded_LowPriority(FOnLyraExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
//}
//
//void AL1TargetPointBase::OnExperienceLoaded(const ULyraExperienceDefinition* ExperienceDefinition)
//{
//	if (HasAuthority() == false)
//		return;
//	
//	AGameStateBase* GameState = UGameplayStatics::GetGameState(GetWorld());
//	UL1ActorSpawningManagerComponent* ActorSpawningManager = GameState->FindComponentByClass<UL1ActorSpawningManagerComponent>();
//	check(ActorSpawningManager);
//	ActorSpawningManager->RegisterTargetPoint(this);
//}
//
//AActor* AL1TargetPointBase::SpawnActor()
//{
//	if (HasAuthority() == false)
//		return nullptr;
//
//	FTransform SpawnTransform = GetActorTransform();
//	SpawnTransform.AddToTranslation(SpawnLocationOffset);
//	
//	AActor* SpawningActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnActorClass, SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
//	InitializeSpawningActor(SpawningActor);
//	SpawningActor->FinishSpawning(SpawnTransform);
//	SpawnedActor = SpawningActor;
//	
//	return SpawningActor;
//}
//
//void AL1TargetPointBase::DestroyActor()
//{
//	if (HasAuthority() == false)
//		return;
//	
//	if (SpawnedActor.Get())
//	{
//		SpawnedActor->Destroy(true);
//	}
//}
