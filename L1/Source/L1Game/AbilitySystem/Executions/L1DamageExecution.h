#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "L1DamageExecution.generated.h"

UCLASS()
class UL1DamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UL1DamageExecution();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
