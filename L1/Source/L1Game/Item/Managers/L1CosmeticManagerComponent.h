#pragma once

#include "L1Define.h"
#include "Components/PawnComponent.h"
#include "L1CosmeticManagerComponent.generated.h"

class AL1ArmorBase;
class UL1ItemFragment_Equipable_Armor;


UCLASS(BlueprintType, Blueprintable)
class UL1CosmeticManagerComponent : public UPawnComponent
{
	GENERATED_BODY()
	
public:
	UL1CosmeticManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
public:
	void RefreshArmorMesh(EArmorType ArmorType/*, const UL1ItemFragment_Equipable_Armor* ArmorFragment*/);
	void GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const;

private:
	void InitializeManager();
	
	UChildActorComponent* SpawnCosmeticSlotActor(TSoftObjectPtr<USkeletalMesh> InDefaultMesh, TSoftObjectPtr<USkeletalMesh> InSecondaryMesh, FName InSkinMaterialSlotName, TSoftObjectPtr<UMaterialInterface> InSkinMaterial);
	void SetPrimaryArmorMesh(EArmorType ArmorType, TSoftObjectPtr<USkeletalMesh> ArmorMeshPtr);
	
protected:
	UPROPERTY(EditDefaultsOnly)
	ECharacterSkinType CharacterSkinType = ECharacterSkinType::Default;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AL1ArmorBase> CosmeticSlotClass;

private:
	UPROPERTY()
	TObjectPtr<UChildActorComponent> HeadSlot;
	
	UPROPERTY()
	TArray<TObjectPtr<UChildActorComponent>> CosmeticSlots;

	bool bInitialized = false;
};
