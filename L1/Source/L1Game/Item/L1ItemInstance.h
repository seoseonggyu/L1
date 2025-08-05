#pragma once

#include "L1Define.h"
#include "AbilitySystem/LyraAbilitySourceInterface.h"
#include "System/L1GameplayTagStack.h"
#include "L1ItemInstance.generated.h"

USTRUCT(BlueprintType)
struct FL1ItemRarityProbability
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	EItemRarity Rarity = EItemRarity::Poor;

	UPROPERTY(EditAnywhere)
	float Probability = 0;
};

UCLASS(BlueprintType)
class UL1ItemInstance : public UObject, public ILyraAbilitySourceInterface
{
	GENERATED_BODY()
	
public:
	UL1ItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual float GetDistanceAttenuation(float Distance, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr) const override;
	virtual float GetPhysicalMaterialAttenuation(const UPhysicalMaterial* PhysicalMaterial, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr) const override;
	
public:
	void Init(int32 InItemTemplateID, EItemRarity InItemRarity);

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddStatTagStack(FGameplayTag StatTag, int32 StackCount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveStatTagStack(FGameplayTag StatTag);

public:
	static EItemRarity DetermineItemRarity(const TArray<FL1ItemRarityProbability>& ItemProbabilities);
	
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetItemTemplateID() const { return ItemTemplateID; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	EItemRarity GetItemRarity() const { return ItemRarity; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasStatTag(FGameplayTag StatTag) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetStackCountByTag(FGameplayTag StatTag) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const FL1GameplayTagStackContainer& GetStatContainer() const { return StatContainer; }

public:
	UFUNCTION(BlueprintCallable, BlueprintPure="false", meta=(DeterminesOutputType="FragmentClass"))
	const UL1ItemFragment* FindFragmentByClass(TSubclassOf<UL1ItemFragment> FragmentClass) const;

	template <typename FragmentClass>
	const FragmentClass* FindFragmentByClass() const
	{
		return (FragmentClass*)FindFragmentByClass(FragmentClass::StaticClass());
	}
	
private:
	UPROPERTY()
	int32 ItemTemplateID = INDEX_NONE;

	UPROPERTY()
	EItemRarity ItemRarity = EItemRarity::Poor;
	
	UPROPERTY()
	FL1GameplayTagStackContainer StatContainer;
};
