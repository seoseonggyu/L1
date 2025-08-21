#include "L1DamageExecution.h"

#include "AbilitySystem/LyraAbilitySourceInterface.h"
#include "AbilitySystem/Attributes/L1VitalSet.h"
#include "AbilitySystem/Attributes/L1CombatSet.h"
#include "AbilitySystem/LyraGameplayEffectContext.h"
#include "Kismet/KismetMathLibrary.h"
#include "Teams/LyraTeamSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1DamageExecution)

struct FDamageStatics
{
public:
	FDamageStatics()
	{
		BaseDamageDef = FGameplayEffectAttributeCaptureDefinition(UL1CombatSet::GetBaseDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		StrengthDef = FGameplayEffectAttributeCaptureDefinition(UL1CombatSet::GetStrengthAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		DefenseDef = FGameplayEffectAttributeCaptureDefinition(UL1CombatSet::GetDefenseAttribute(), EGameplayEffectAttributeCaptureSource::Target, true);
	}

public:
	FGameplayEffectAttributeCaptureDefinition BaseDamageDef;
	FGameplayEffectAttributeCaptureDefinition StrengthDef;
	FGameplayEffectAttributeCaptureDefinition DefenseDef;
};

static FDamageStatics& DamageStatics()
{
	static FDamageStatics Statics;
	return Statics;
}

UL1DamageExecution::UL1DamageExecution()
{
	RelevantAttributesToCapture.Add(DamageStatics().BaseDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().StrengthDef);
	RelevantAttributesToCapture.Add(DamageStatics().DefenseDef);
}

void UL1DamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FLyraGameplayEffectContext* TypedContext = FLyraGameplayEffectContext::ExtractEffectContext(Spec.GetContext());
	check(TypedContext);

	/*
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;
	
	float BaseDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BaseDamageDef, EvaluateParameters, BaseDamage);

	float Strength = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().StrengthDef, EvaluateParameters, Strength);

	float Defense = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DefenseDef, EvaluateParameters, Defense);
	*/

	const AActor* EffectCauser = TypedContext->GetEffectCauser();
	const FHitResult* HitActorResult = TypedContext->GetHitResult();
	AActor* HitActor = nullptr;
	
	if (HitActorResult)
	{
		const FHitResult& CurrHitResult = *HitActorResult;
		HitActor = CurrHitResult.HitObjectHandle.FetchActor();
	}
	
	// float Damage = UKismetMathLibrary::SafeDivide(BaseDamage + Strength, FMath::Pow(FMath::Max(Defense, 1.f), 0.3f));

	/*
	// Apply rules for team damage
	float DamageInteractionAllowedMultiplier = 0.0f;
	if (HitActor)
	{
		ULyraTeamSubsystem* TeamSubsystem = HitActor->GetWorld()->GetSubsystem<ULyraTeamSubsystem>();
		if (ensure(TeamSubsystem))
		{
			DamageInteractionAllowedMultiplier = TeamSubsystem->CanCauseDamage(EffectCauser, HitActor) ? 1.f : 0.f;
		}
	}
	
	// Weak point damage calculation
	float PhysicalMaterialAttenuation = 1.0f;
	if (const ILyraAbilitySourceInterface* AbilitySource = TypedContext->GetAbilitySource())
	{
		if (const UPhysicalMaterial* PhysicalMaterial = TypedContext->GetPhysicalMaterial())
		{
			PhysicalMaterialAttenuation = AbilitySource->GetPhysicalMaterialAttenuation(PhysicalMaterial, SourceTags, TargetTags);
		}
	}
	
	float DamageDone = FMath::Max(Damage * PhysicalMaterialAttenuation * DamageInteractionAllowedMultiplier, 0.0f);
	DamageDone -= DamageDone * 100.f;
	*/

	// TODO: 데미지 숫자 처리
	float DamageDone = 20.f;
	if (DamageDone > 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UL1VitalSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, DamageDone));

		
		//if (UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent())
		//{
		//	
		//	 SourceASC->ApplyModToAttribute(UL1VitalSet::GetHealthAttribute(), EGameplayModOp::Additive, DamageDone /** (DrainLifePercent / 100.f)*/);
		//}
	}
}
