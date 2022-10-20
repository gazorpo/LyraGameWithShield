// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "LyraShieldRechargeExecution.generated.h"


/**
 * ULyraShieldRechargeExecution
 *
 *	Execution used by gameplay effects to apply shield recharge to shield attribute.
 */
UCLASS()
class ULyraShieldRechargeExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	ULyraShieldRechargeExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
