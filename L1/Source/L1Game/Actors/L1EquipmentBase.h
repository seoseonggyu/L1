#pragma once

#include "AbilitySystemInterface.h"
#include "L1Define.h"
#include "AbilitySystem/LyraAbilitySet.h"
#include "L1EquipmentBase.generated.h"

class UAbilitySystemComponent;
class USkeletalMeshComponent;
class UArrowComponent;
class UBoxComponent;

UCLASS(BlueprintType, Abstract)
class AL1EquipmentBase : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AL1EquipmentBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

public:
	void Init(int32 InTemplateID, EEquipmentSlotType InEquipmentSlotType);

	UFUNCTION(BlueprintCallable)
	void ChangeBlockState(bool bShouldBlock);

private:
	UFUNCTION()
	void EquipmentSlot();

	UFUNCTION()
	void CanBlock();

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	int32 GetTemplateID() const { return TemplateID; }
	EEquipmentSlotType GetEquipmentSlotType() const { return EquipmentSlotType; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UAnimMontage* GetEquipMontage();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UAnimMontage* GetHitMontage(AActor* InstigatorActor, const FVector& HitLocation, bool IsBlocked);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UArrowComponent> ArrowComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> TraceDebugCollision;

protected:
	UPROPERTY()
	int32 TemplateID;

	UPROPERTY()
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

public:
	UPROPERTY()
	bool bCanBlock = false;

private:
	FLyraAbilitySet_GrantedHandles SkillAbilitySetHandles;
};