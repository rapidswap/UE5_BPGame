// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "BPCombatCharacterBase.generated.h"

// 전방 선언.
class UAbilitySystemComponent;
class UAnimSequenceBase;
class UBPCombatAttributeSet;
class UBPAttackGameplayAbility;
class UGameplayEffect;
struct FOnAttributeChangeData;

UCLASS(Abstract)
class BPGAME_API ABPCombatCharacterBase : public ACharacter,public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABPCombatCharacterBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void ApplyInitialStats();
	void GrantAttackAbility();

	void HandleHealthChaged(const FOnAttributeChangeData& Data);

	UFUNCTION(BlueprintImplementableEvent,Category="GAS|Combat", meta = (DisplayName="On Gas Death"))
	void OnGasDeath();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UBPCombatAttributeSet> CombatAttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS|Initialization")
	TSubclassOf<UGameplayEffect> InitialStatsEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS|Combat")
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS|Combat")
	TSubclassOf<UBPAttackGameplayAbility> AttackAbilityClass;

	/** 몽타주 또는 애니메이션 시퀀스. 시퀀스는 런타임에 Dynamic Montage로 재생한다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS|Combat")
	TObjectPtr<UAnimSequenceBase> AttackAnimation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS|Combat")
	FName AttackSlotName = TEXT("DefaultSlot");

	UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly,Category="GAS|Combat")
	bool bIsDead = false;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable,Category="GAS|Combat")
	bool ApplyDamageEffectToTarget(AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category="GAS|Combat")
	bool TryActivateAttackAbility();

	UFUNCTION(BlueprintImplementableEvent, Category="GAS|Combat", meta=(DisplayName="On Gas Attack Started"))
	void OnGasAttackStarted();

	UFUNCTION(BlueprintImplementableEvent, Category="GAS|Combat", meta=(DisplayName="On Gas Attack Ended"))
	void OnGasAttackEnded(bool bWasCancelled);

	UAnimSequenceBase* GetAttackAnimation() const { return AttackAnimation; }
	FName GetAttackSlotName() const { return AttackSlotName; }



};
