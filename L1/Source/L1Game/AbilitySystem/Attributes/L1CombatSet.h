#pragma once

#include "AbilitySystemComponent.h"
#include "L1AttributeSet.h"
#include "L1CombatSet.generated.h"

UCLASS(BlueprintType)
class UL1CombatSet : public UL1AttributeSet
{
	GENERATED_BODY()

public:
	UL1CombatSet();

protected:
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

private:
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
	
public:
	ATTRIBUTE_ACCESSORS(ThisClass, BaseDamage);
	ATTRIBUTE_ACCESSORS(ThisClass, BaseHealth);
	ATTRIBUTE_ACCESSORS(ThisClass, BaseMana);


private:
	UPROPERTY(BlueprintReadOnly, meta=(HideFromModifiers, AllowPrivateAccess="true"))
	FGameplayAttributeData BaseDamage;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData BaseHealth;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData BaseMana;
public:
	ATTRIBUTE_ACCESSORS(ThisClass, Strength);
	ATTRIBUTE_ACCESSORS(ThisClass, Defense);
	ATTRIBUTE_ACCESSORS(ThisClass, Vigor);
	ATTRIBUTE_ACCESSORS(ThisClass, Agility);
	ATTRIBUTE_ACCESSORS(ThisClass, Resourcefulness);
	

private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Strength;
	
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Defense;
	
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Vigor;
	
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Agility;
	
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Resourcefulness;

public:
	ATTRIBUTE_ACCESSORS(ThisClass, ActiveEffectDuration);

private:
	UPROPERTY(BlueprintReadOnly, meta=(HideFromModifiers, AllowPrivateAccess="true"))
	FGameplayAttributeData ActiveEffectDuration;
};
