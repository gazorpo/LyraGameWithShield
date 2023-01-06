// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "LyraAttributeSet.h"
#include "Misc/AssertionMacros.h"
#include "NativeGameplayTags.h"
#include "UObject/Class.h"
#include "UObject/UObjectGlobals.h"

#include "LyraHealthSet.generated.h"

class UObject;
struct FFrame;

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_Damage);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageImmunity);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageSelfDestruct);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_FellOutOfWorld);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Lyra_Damage_Message);

struct FGameplayEffectModCallbackData;


/**
 * ULyraHealthSet
 *
 *	Class that defines attributes that are necessary for taking damage.
 *	Attribute examples include: health, shields, and resistances.
 */
UCLASS(BlueprintType)
class ULyraHealthSet : public ULyraAttributeSet
{
	GENERATED_BODY()

public:

	ULyraHealthSet();

	ATTRIBUTE_ACCESSORS(ULyraHealthSet, Health);
	ATTRIBUTE_ACCESSORS(ULyraHealthSet, Shield);
	ATTRIBUTE_ACCESSORS(ULyraHealthSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(ULyraHealthSet, MaxShield);
	ATTRIBUTE_ACCESSORS(ULyraHealthSet, Healing);
	ATTRIBUTE_ACCESSORS(ULyraHealthSet, ShieldRegen);
	ATTRIBUTE_ACCESSORS(ULyraHealthSet, Damage);

	// Delegate to broadcast when the health attribute reaches zero.
	mutable FLyraAttributeEvent OnOutOfHealth;

	// Delegate to broadcast when the shield attribute reaches zero.
	mutable FLyraAttributeEvent OnOutOfShield;

protected:

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Shield(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxShield(const FGameplayAttributeData& OldValue);

	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:

	// The current health attribute.  The health will be capped by the max health attribute.  Health is hidden from modifiers so only executions can modify it.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Lyra|Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	// The current shield attribute.  The shield will be capped by the max shield attribute.  Shield is hidden from modifiers so only executions can modify it.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Shield, Category = "Lyra|Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Shield;

	// The current max health attribute.  Max health is an attribute since gameplay effects can modify it.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Lyra|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	// The current max shield attribute.  Max shield is an attribute since gameplay effects can modify it.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxShield, Category = "Lyra|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxShield;

	// Used to track when the health reaches 0.
	bool bOutOfHealth;

	// Used to track when the shield reaches 0.
	bool bOutOfShield;

	// -------------------------------------------------------------------
	//	Meta Attribute (please keep attributes that aren't 'stateful' below 
	// -------------------------------------------------------------------

private:
	// Incoming healing. This is mapped directly to +Health
	UPROPERTY(BlueprintReadOnly, Category="Lyra|Health", Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData Healing;

	// Incoming shield regeneration. This is mapped directly to +Shield
	UPROPERTY(BlueprintReadOnly, Category = "Lyra|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ShieldRegen;

	// Incoming damage. This is mapped directly to -Shield, and then any remaining is mapped directly to -Health
	UPROPERTY(BlueprintReadOnly, Category="Lyra|Health", Meta=(HideFromModifiers, AllowPrivateAccess=true))
	FGameplayAttributeData Damage;
};
