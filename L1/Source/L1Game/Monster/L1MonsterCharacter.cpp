#include "Monster/L1MonsterCharacter.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/L1CombatSet.h"
#include "AbilitySystem/Attributes/L1VitalSet.h"

AL1MonsterCharacter::AL1MonsterCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{


	AbilitySystemComponent = CreateDefaultSubobject<ULyraAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	HealthSet = CreateDefaultSubobject<UL1VitalSet>(TEXT("VitalSet"));
	CombatSet = CreateDefaultSubobject<UL1CombatSet>(TEXT("CombatSet"));
}

void AL1MonsterCharacter::BeginPlay()
{
	Super::BeginPlay();
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void AL1MonsterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

ULyraAbilitySystemComponent* AL1MonsterCharacter::GetLyraAbilitySystemComponent() const
{
	return Cast<ULyraAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* AL1MonsterCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
