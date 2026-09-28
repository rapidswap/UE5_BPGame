// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "BPCombatAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class BPGAME_API UBPCombatAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UBPCombatAttributeSet();

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health,Category="Attributes")
	FGameplayAttributeData Health;

	ATTRIBUTE_ACCESSORS_BASIC(UBPCombatAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Attributes")
	FGameplayAttributeData MaxHealth;

	ATTRIBUTE_ACCESSORS_BASIC(UBPCombatAttributeSet, MaxHealth)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackPower, Category = "Attributes")
	FGameplayAttributeData AttackPower;

	ATTRIBUTE_ACCESSORS_BASIC(UBPCombatAttributeSet, AttackPower)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Damage;

	ATTRIBUTE_ACCESSORS_BASIC(UBPCombatAttributeSet, Damage)

protected:
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AttackPower(const FGameplayAttributeData& OldValue);
};
