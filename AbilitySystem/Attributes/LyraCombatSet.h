// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "LyraAttributeSet.h"
#include "Misc/AssertionMacros.h"
#include "UObject/Class.h"
#include "UObject/UObjectGlobals.h"

#include "LyraCombatSet.generated.h"

class UObject;
struct FFrame;


/**
 * ULyraCombatSet
 *
 *  Class that defines attributes that are necessary for applying damage or healing.
 *	Attribute examples include: damage, healing, attack power, and shield penetrations.
 */
UCLASS(BlueprintType)
class ULyraCombatSet : public ULyraAttributeSet
{
	GENERATED_BODY()

public:

	ULyraCombatSet();

	ATTRIBUTE_ACCESSORS(ULyraCombatSet, BaseDamage);
	ATTRIBUTE_ACCESSORS(ULyraCombatSet, BaseHeal);
	ATTRIBUTE_ACCESSORS(ULyraCombatSet, BaseShieldRecharge);

protected:

	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BaseHeal(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BaseShieldRecharge(const FGameplayAttributeData& OldValue);

private:

	// The base amount of damage to apply in the damage execution.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, Category = "Lyra|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseDamage;

	// The base amount of healing to apply in the heal execution.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseHeal, Category = "Lyra|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseHeal;

	// The base amount of shield recharge to apply in the shield recharge execution.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseShieldRecharge, Category = "Lyra|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseShieldRecharge;
};
