#pragma once

#include "AttributeSet.h"
#include "L1AttributeSet.generated.h"

class ULyraAbilitySystemComponent;
struct FGameplayEffectSpec;

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)				\
		GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName)	\
		GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)				\
		GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)				\
		GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_MULTICAST_DELEGATE_SixParams(FLyraAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/, const FGameplayEffectSpec* /*EffectSpec*/, float /*EffectMagnitude*/, float /*OldValue*/, float /*NewValue*/);

UCLASS()
class L1GAME_API UL1AttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UL1AttributeSet();

public:
	UWorld* GetWorld() const override;
	ULyraAbilitySystemComponent* GetLyraAbilitySystemComponent() const;
};
