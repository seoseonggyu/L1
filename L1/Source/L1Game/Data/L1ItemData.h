#pragma once

#include "L1ItemData.generated.h"

class UL1ItemTemplate;

UCLASS(BlueprintType, Const, meta=(DisplayName="L1 Item Data"))
class UL1ItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	static const UL1ItemData& Get();

public:
#if WITH_EDITORONLY_DATA
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
#endif // WITH_EDITORONLY_DATA
	
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif // WITH_EDITOR
	
public:
	const UL1ItemTemplate& FindItemTemplateByID(int32 ItemTemplateID) const;
	int32 FindItemTemplateIDByClass(TSubclassOf<UL1ItemTemplate> ItemTemplateClass) const;
	
	void GetAllItemTemplateClasses(TArray<TSubclassOf<UL1ItemTemplate>>& OutItemTemplateClasses) const;
	const TArray<TSubclassOf<UL1ItemTemplate>>& GetWeaponItemTemplateClasses() const { return WeaponItemTemplateClasses; }
	const TArray<TSubclassOf<UL1ItemTemplate>>& GetArmorItemTemplateClasses() const { return ArmorItemTemplateClasses; }
	
private:
	UPROPERTY(EditDefaultsOnly)
	TMap<int32, TSubclassOf<UL1ItemTemplate>> ItemTemplateIDToClass;

	UPROPERTY()
	TMap<TSubclassOf<UL1ItemTemplate>, int32> ItemTemplateClassToID;

private:
	UPROPERTY()
	TArray<TSubclassOf<UL1ItemTemplate>> WeaponItemTemplateClasses;

	UPROPERTY()
	TArray<TSubclassOf<UL1ItemTemplate>> ArmorItemTemplateClasses;
};
