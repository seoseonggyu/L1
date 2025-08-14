#include "L1AttributeSet.h"

#include "AbilitySystem/LyraAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1AttributeSet)

UL1AttributeSet::UL1AttributeSet()
{
	
}

UWorld* UL1AttributeSet::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}

ULyraAbilitySystemComponent* UL1AttributeSet::GetLyraAbilitySystemComponent() const
{
	return Cast<ULyraAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}
