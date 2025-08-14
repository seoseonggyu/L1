#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "L1VitalExecution.generated.h"

UCLASS()
class UL1VitalExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UL1VitalExecution();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
