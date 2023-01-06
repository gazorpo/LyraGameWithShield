// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraHealthSet.h"
#include "LyraGameplayTags.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Engine/World.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffectTypes.h"
#include "Messages/LyraVerbMessage.h"
#include "GameFramework/GameplayMessageSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraHealthSet)

UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_Damage, "Gameplay.Damage");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_DamageImmunity, "Gameplay.DamageImmunity");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_DamageSelfDestruct, "Gameplay.Damage.SelfDestruct");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_FellOutOfWorld, "Gameplay.Damage.FellOutOfWorld");
UE_DEFINE_GAMEPLAY_TAG(TAG_Lyra_Damage_Message, "Lyra.Damage.Message");

ULyraHealthSet::ULyraHealthSet()
	: Health(100.0f)
	, Shield(100.0f)
	, MaxHealth(100.0f)
	, MaxShield(100.0f)
{
	bOutOfHealth = false;
	bOutOfShield = false;
}

void ULyraHealthSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ULyraHealthSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULyraHealthSet, Shield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULyraHealthSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULyraHealthSet, MaxShield, COND_None, REPNOTIFY_Always);
}

void ULyraHealthSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULyraHealthSet, Health, OldValue);
}

void ULyraHealthSet::OnRep_Shield(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULyraHealthSet, Shield, OldValue);
}

void ULyraHealthSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULyraHealthSet, MaxHealth, OldValue);
}

void ULyraHealthSet::OnRep_MaxShield(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULyraHealthSet, MaxShield, OldValue);
}

bool ULyraHealthSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData &Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	// Handle modifying incoming normal damage
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		if (Data.EvaluatedData.Magnitude > 0.0f)
		{
			const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(TAG_Gameplay_DamageSelfDestruct);

			if (Data.Target.HasMatchingGameplayTag(TAG_Gameplay_DamageImmunity) && !bIsDamageFromSelfDestruct)
			{
				// Do not take away any health.
				Data.EvaluatedData.Magnitude = 0.0f;
				return false;
			}

#if !UE_BUILD_SHIPPING
			// Check GodMode cheat, unlimited health is checked below
			if (Data.Target.HasMatchingGameplayTag(FLyraGameplayTags::Get().Cheat_GodMode) && !bIsDamageFromSelfDestruct)
			{
				// Do not take away any health.
				Data.EvaluatedData.Magnitude = 0.0f;
				return false;
			}
#endif // #if !UE_BUILD_SHIPPING
		}
	}

	return true;
}

void ULyraHealthSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(TAG_Gameplay_DamageSelfDestruct);
	float MinimumHealth = 0.0f;

#if !UE_BUILD_SHIPPING
	// Godmode and unlimited health stop death unless it's a self destruct
	if (!bIsDamageFromSelfDestruct &&
		(Data.Target.HasMatchingGameplayTag(FLyraGameplayTags::Get().Cheat_GodMode) || Data.Target.HasMatchingGameplayTag(FLyraGameplayTags::Get().Cheat_UnlimitedHealth) ))
	{
		MinimumHealth = 1.0f;
	}
#endif // #if !UE_BUILD_SHIPPING

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// Send a standardized verb message that other systems can observe
		if (Data.EvaluatedData.Magnitude < 0.0f)
		{
			FLyraVerbMessage Message;
			Message.Verb = TAG_Lyra_Damage_Message;
			Message.Instigator = Data.EffectSpec.GetEffectContext().GetEffectCauser();
			Message.InstigatorTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
			Message.Target = GetOwningActor();
			Message.TargetTags = *Data.EffectSpec.CapturedTargetTags.GetAggregatedTags();
			//@TODO: Fill out context tags, and any non-ability-system source/instigator tags
			//@TODO: Determine if it's an opposing team kill, self-own, team kill, etc...
			Message.Magnitude = Data.EvaluatedData.Magnitude;

			UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
			MessageSystem.BroadcastMessage(Message.Verb, Message);
		}

		// Convert into -Shield and -Health and then clamp
		const float StartingShield = GetShield();
		const float TotalDamage = GetDamage();
		const float ShieldDamage = FMath::Clamp(TotalDamage, 0.0f, StartingShield);
		SetShield(FMath::Clamp(StartingShield - ShieldDamage, 0.0f, GetMaxShield()));
		SetHealth(FMath::Clamp(GetHealth() - (TotalDamage - ShieldDamage), MinimumHealth, GetMaxHealth()));
		SetDamage(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		// Convert into +Health and then clamo
		SetHealth(FMath::Clamp(GetHealth() + GetHealing(), MinimumHealth, GetMaxHealth()));
		SetHealing(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetShieldRegenAttribute())
	{
		// Convert into +Shield and then clamo
		SetShield(FMath::Clamp(GetShield() + GetShieldRegen(), 0.0f, GetMaxShield()));
		SetShieldRegen(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Clamp and fall into out of health handling below
		SetHealth(FMath::Clamp(GetHealth(), MinimumHealth, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetShieldAttribute())
	{
		// Clamp and fall into out of shield handling below
		SetShield(FMath::Clamp(GetShield(), 0.0f, GetMaxShield()));
	}

	if ((GetHealth() <= 0.0f) && !bOutOfHealth)
	{
		if (OnOutOfHealth.IsBound())
		{
			const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
			AActor* Instigator = EffectContext.GetOriginalInstigator();
			AActor* Causer = EffectContext.GetEffectCauser();

			OnOutOfHealth.Broadcast(Instigator, Causer, Data.EffectSpec, Data.EvaluatedData.Magnitude);
		}
	}

	if ((GetShield() <= 0.0f) && !bOutOfShield)
	{
		if (OnOutOfShield.IsBound())
		{
			const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
			AActor* Instigator = EffectContext.GetOriginalInstigator();
			AActor* Causer = EffectContext.GetEffectCauser();

			OnOutOfShield.Broadcast(Instigator, Causer, Data.EffectSpec, Data.EvaluatedData.Magnitude);
		}
	}

	// Check health again in case an event above changed it.
	bOutOfHealth = (GetHealth() <= 0.0f);

	// Check shield again in case an event above changed it.
	bOutOfShield = (GetShield() <= 0.0f);
}

void ULyraHealthSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void ULyraHealthSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void ULyraHealthSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		// Make sure current health is not greater than the new max health.
		if (GetHealth() > NewValue)
		{
			ULyraAbilitySystemComponent* LyraASC = GetLyraAbilitySystemComponent();
			check(LyraASC);

			LyraASC->ApplyModToAttribute(GetHealthAttribute(), EGameplayModOp::Override, NewValue);
		}
	}
	else if (Attribute == GetMaxShieldAttribute())
	{
		// Make sure current shield is not greater than the new max shield.
		if (GetShield() > NewValue)
		{
			ULyraAbilitySystemComponent* LyraASC = GetLyraAbilitySystemComponent();
			check(LyraASC);

			LyraASC->ApplyModToAttribute(GetShieldAttribute(), EGameplayModOp::Override, NewValue);
		}
	}

	if (bOutOfHealth && (GetHealth() > 0.0f))
	{
		bOutOfHealth = false;
	}

	if (bOutOfShield && (GetShield() > 0.0f))
	{
		bOutOfShield = false;
	}
}

void ULyraHealthSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		// Do not allow health to go negative or above max health.
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetShieldAttribute())
	{
		// Do not allow shield to go negative or above max shield.
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxShield());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		// Do not allow max health to drop below 1.
		NewValue = FMath::Max(NewValue, 1.0f);
	}
	else if (Attribute == GetMaxShieldAttribute())
	{
		// Do not allow max shield to drop below 0.
		NewValue = FMath::Max(NewValue, 0.0f);
	}
}

