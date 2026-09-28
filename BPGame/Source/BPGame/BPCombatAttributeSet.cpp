// Fill out your copyright notice in the Description page of Project Settings.


#include "BPCombatAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UBPCombatAttributeSet::UBPCombatAttributeSet()
{
	InitMaxHealth(100.0f);
	InitHealth(100.0f);
	InitAttackPower(10.0f);
	InitDamage(0.0f);
}

void UBPCombatAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
	else if (Attribute == GetAttackPowerAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);
	}
}

void UBPCombatAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float IncomingDamage = GetDamage();

		// Damage는 계산 후 반드시 0으로 초기화한다.
		SetDamage(0.0f);

		if (IncomingDamage > 0.0f && GetHealth() > 0.0f)
		{
			SetHealth(FMath::Clamp(GetHealth() - IncomingDamage, 0.0f, GetMaxHealth()));
		}
		else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
		{
			SetHealth(FMath::Clamp(GetHealth() - IncomingDamage, 0.0f, GetMaxHealth()));
		}
		else if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
		{
			SetMaxHealth(FMath::Max(GetMaxHealth(), 1.0f));

			SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
		}
		else if (Data.EvaluatedData.Attribute == GetAttackPowerAttribute())
		{
			SetAttackPower(FMath::Max(GetAttackPower(), 0.0f));
		}
	}
}

void UBPCombatAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(
		UBPCombatAttributeSet,
		Health,
		COND_None,
		REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(
		UBPCombatAttributeSet,
		MaxHealth,
		COND_None,
		REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(
		UBPCombatAttributeSet,
		AttackPower,
		COND_None,
		REPNOTIFY_Always);
}

void UBPCombatAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBPCombatAttributeSet, Health, OldValue);
}

void UBPCombatAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBPCombatAttributeSet, MaxHealth, OldValue);
}

void UBPCombatAttributeSet::OnRep_AttackPower(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBPCombatAttributeSet, AttackPower, OldValue);
}
