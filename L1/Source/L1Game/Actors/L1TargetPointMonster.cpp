//#include "L1TargetPointMonster.h"
//
//#include "AIController.h"
//#include "Character/LyraPawnExtensionComponent.h"
//#include "GameModes/LyraGameMode.h"
//#include "Kismet/GameplayStatics.h"
//
//#include UE_INLINE_GENERATED_CPP_BY_NAME(L1TargetPointMonster)
//
//AL1TargetPointMonster::AL1TargetPointMonster(const FObjectInitializer& ObjectInitializer)
//	: Super(ObjectInitializer)
//{
//	TargetPointType = EL1TargetPointType::Monster;
//}
//
//void AL1TargetPointMonster::InitializeSpawningActor(AActor* InSpawningActor)
//{
//	Super::InitializeSpawningActor(InSpawningActor);
//	
//	FActorSpawnParameters SpawnParameters;
//	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
//	SpawnParameters.OverrideLevel = InSpawningActor->GetLevel();
//	SpawnParameters.ObjectFlags |= RF_Transient;
//
//	APawn* SpawningPawn = Cast<APawn>(InSpawningActor);
//	if (SpawningPawn == nullptr)
//		return;
//
//	AAIController* NewController = GetWorld()->SpawnActor<AAIController>(SpawningPawn->AIControllerClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParameters);
//	if (NewController == nullptr)
//		return;
//	
//	NewController->Possess(SpawningPawn);
//
//	if (APawn* ControlledPawn = NewController->GetPawn())
//	{
//		if (ULyraPawnExtensionComponent* PawnExtensionComponent = ControlledPawn->FindComponentByClass<ULyraPawnExtensionComponent>())
//		{
//			ALyraGameMode* LyraGameMode = Cast<ALyraGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
//			check(LyraGameMode);
//			
//			if (const ULyraPawnData* PawnData = LyraGameMode->GetPawnDataForController(NewController))
//			{
//				PawnExtensionComponent->SetPawnData(PawnData);
//			}
//		}
//	}
//}
