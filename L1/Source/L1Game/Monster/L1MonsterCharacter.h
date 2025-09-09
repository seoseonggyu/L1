#pragma once

#include "Character/LyraCharacter.h"
#include "L1MonsterCharacter.generated.h"

class ULyraAbilitySystemComponent;

UCLASS()
class L1GAME_API AL1MonsterCharacter : public ALyraCharacter
{
	GENERATED_BODY()
	
public:
	AL1MonsterCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	virtual ULyraAbilitySystemComponent* GetLyraAbilitySystemComponent() const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY(VisibleAnywhere, Category = "L1|NetworkCharacter")
	TObjectPtr<ULyraAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<const class UL1VitalSet> HealthSet;
	UPROPERTY()
	TObjectPtr<const class UL1StatSet> StatSet;
};
