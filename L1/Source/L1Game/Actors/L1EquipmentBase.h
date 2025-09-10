#pragma once

#include "AbilitySystemInterface.h"
#include "L1Define.h"
#include "AbilitySystem/LyraAbilitySet.h"
#include "L1EquipmentBase.generated.h"

class UL1ItemInstance;
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

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void ChangeBlockState(bool bShouldBlock);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ProcessEquip(UL1ItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable)
	void PlayEquipMontage();

private:
	void CheckPropertyInitialization();

	UFUNCTION()
	void ProcessItemTemplateID();

	UFUNCTION()
	void ProcessEquipmentSlotType();

	UFUNCTION()
	void ProcessCanBlock();

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	int32 GetItemTemplateID() const { return ItemTemplateID; }
	EEquipmentSlotType GetEquipmentSlotType() const { return EquipmentSlotType; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TSoftObjectPtr<UAnimMontage> GetEquipMontage() const;

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
	int32 ItemTemplateID = INDEX_NONE;

	UPROPERTY()
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

public:
	UPROPERTY()
	bool bCanBlock = false;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bInitialized = false;

private:
	FLyraAbilitySet_GrantedHandles SkillAbilitySetHandles;
};
