#include "L1NetworkCharacter.h"

#include "AbilitySystem/LyraAbilitySystemComponent.h"

AL1NetworkCharacter::AL1NetworkCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = CreateDefaultSubobject<ULyraAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}

void AL1NetworkCharacter::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void AL1NetworkCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

ULyraAbilitySystemComponent* AL1NetworkCharacter::GetLyraAbilitySystemComponent() const
{
	return Cast<ULyraAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* AL1NetworkCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
