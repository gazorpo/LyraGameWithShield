// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraShieldRechargeExecution.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystem/Attributes/LyraHealthSet.h"
#include "AbilitySystem/Attributes/LyraCombatSet.h"


struct FShieldRechargeStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseShieldRechargeDef;

	FShieldRechargeStatics()
	{
		BaseShieldRechargeDef = FGameplayEffectAttributeCaptureDefinition(ULyraCombatSet::GetBaseShieldRechargeAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FShieldRechargeStatics& ShieldRechargeStatics()
{
	static FShieldRechargeStatics Statics;
	return Statics;
}


ULyraShieldRechargeExecution::ULyraShieldRechargeExecution()
{
	RelevantAttributesToCapture.Add(ShieldRechargeStatics().BaseShieldRechargeDef);
}

void ULyraShieldRechargeExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseShieldRecharge = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ShieldRechargeStatics().BaseShieldRechargeDef, EvaluateParameters, BaseShieldRecharge);

	const float ShieldRechargeDone = FMath::Max(0.0f, BaseShieldRecharge);

	if (ShieldRechargeDone > 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(ULyraHealthSet::GetShieldAttribute(), EGameplayModOp::Additive, ShieldRechargeDone));
	}
#endif // #if WITH_SERVER_CODE
}
