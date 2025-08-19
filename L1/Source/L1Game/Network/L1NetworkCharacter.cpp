#include "L1NetworkCharacter.h"

#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/L1CombatSet.h"
#include "AbilitySystem/Attributes/L1VitalSet.h"

AL1NetworkCharacter::AL1NetworkCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = CreateDefaultSubobject<ULyraAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	// These attribute sets will be detected by AbilitySystemComponent::InitializeComponent. Keeping a reference so that the sets don't get garbage collected before that.
	HealthSet = CreateDefaultSubobject<UL1VitalSet>(TEXT("VitalSet"));
	CombatSet = CreateDefaultSubobject<UL1CombatSet>(TEXT("CombatSet"));
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
