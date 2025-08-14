#pragma once

#include "AbilitySystemComponent.h"
#include "L1AttributeSet.h"
#include "NativeGameplayTags.h"
#include "L1VitalSet.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_Damage);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageImmunity);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageSelfDestruct);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_FellOutOfWorld);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Lyra_Damage_Message);

UCLASS(BlueprintType)
class UL1VitalSet : public UL1AttributeSet
{
	GENERATED_BODY()

public:
	UL1VitalSet();

protected:
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

private:
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
	
public:
	ATTRIBUTE_ACCESSORS(ThisClass, Health);
	ATTRIBUTE_ACCESSORS(ThisClass, MaxHealth);
	ATTRIBUTE_ACCESSORS(ThisClass, Mana);
	ATTRIBUTE_ACCESSORS(ThisClass, MaxMana);


private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Health;
	
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData MaxHealth;
	
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Mana;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData MaxMana;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Stamina;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData MaxStamina;

public:
	ATTRIBUTE_ACCESSORS(ThisClass, IncomingHealth);
	ATTRIBUTE_ACCESSORS(ThisClass, IncomingMana);
	ATTRIBUTE_ACCESSORS(ThisClass, IncomingDamage);
	
private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData IncomingHealth;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData IncomingMana;
	
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData IncomingDamage;

public:
	mutable FLyraAttributeEvent OnHealthChanged;
	mutable FLyraAttributeEvent OnMaxHealthChanged;
	mutable FLyraAttributeEvent OnOutOfHealth;

private:
	bool bOutOfHealth;
	
	float MaxHealthBeforeAttributeChange;
	float HealthBeforeAttributeChange;
};
