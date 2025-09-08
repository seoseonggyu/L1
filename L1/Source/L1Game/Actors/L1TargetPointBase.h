//#pragma once
//
//#include "Engine/TargetPoint.h"
//#include "L1TargetPointBase.generated.h"
//
//class AAIController;
//
//UENUM(BlueprintType)
//enum class EL1TargetPointType : uint8
//{
//	None,
//	Statue,
//	Chest,
//	Monster,
//};
//
//UCLASS()
//class AL1TargetPointBase : public ATargetPoint
//{
//	GENERATED_BODY()
//	
//public:
//	AL1TargetPointBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
//
//protected:
//	virtual void BeginPlay() override;
//	virtual void InitializeSpawningActor(AActor* InSpawningActor) { }
//	
//public:
//	AActor* SpawnActor();
//	void DestroyActor();
//
//	AActor* GetSpawnedActor() const { return SpawnedActor.Get(); };
//	
//private:
//	UFUNCTION()
//	void OnExperienceLoaded(const ULyraExperienceDefinition* ExperienceDefinition);
//
//public:
//	UPROPERTY(EditDefaultsOnly, Category="TargetPoint")
//	EL1TargetPointType TargetPointType = EL1TargetPointType::None;
//
//	UPROPERTY(EditDefaultsOnly, Category="TargetPoint")
//	bool bSpawnWhenDestroyed = false;
//	
//	UPROPERTY(EditDefaultsOnly, Category="TargetPoint")
//	TSubclassOf<AActor> SpawnActorClass;
//
//	UPROPERTY(EditDefaultsOnly, Category="TargetPoint")
//	FVector SpawnLocationOffset = FVector::ZeroVector;
//
//protected:
//	UPROPERTY(VisibleAnywhere)
//	TObjectPtr<UStaticMeshComponent> PreviewMeshComponent;
//	
//protected:
//	UPROPERTY()
//	TWeakObjectPtr<AActor> SpawnedActor;
//};
