#include "L1VitalExecution.h"

#include "AbilitySystem/Attributes/L1VitalSet.h"
#include "AbilitySystem/Attributes/L1CombatSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1VitalExecution)

struct FVitalStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseHealthDef;
	FGameplayEffectAttributeCaptureDefinition BaseManaDef;
	FGameplayEffectAttributeCaptureDefinition BaseStaminaDef;

	FVitalStatics()
	{
		BaseHealthDef = FGameplayEffectAttributeCaptureDefinition(UL1CombatSet::GetBaseHealthAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		BaseManaDef = FGameplayEffectAttributeCaptureDefinition(UL1CombatSet::GetBaseManaAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FVitalStatics& HealStatics()
{
	static FVitalStatics Statics;
	return Statics;
}

UL1VitalExecution::UL1VitalExecution()
{
	RelevantAttributesToCapture.Add(HealStatics().BaseHealthDef);
	RelevantAttributesToCapture.Add(HealStatics().BaseManaDef);
	RelevantAttributesToCapture.Add(HealStatics().BaseStaminaDef);
}

void UL1VitalExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseHealth = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealStatics().BaseHealthDef, EvaluateParameters, BaseHealth);

	float BaseMana = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealStatics().BaseManaDef, EvaluateParameters, BaseMana);
	
	float BaseStamina = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealStatics().BaseStaminaDef, EvaluateParameters, BaseStamina);

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UL1VitalSet::GetIncomingHealthAttribute(), EGameplayModOp::Additive, BaseHealth));
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UL1VitalSet::GetIncomingManaAttribute(), EGameplayModOp::Additive, BaseMana));
	
}
