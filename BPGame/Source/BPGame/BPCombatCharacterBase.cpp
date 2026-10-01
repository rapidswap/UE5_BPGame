// Fill out your copyright notice in the Description page of Project Settings.


#include "BPCombatCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "BPAttackGameplayAbility.h"
#include "BPCombatAttributeSet.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayEffect.h"
#include "AbilitySystemGlobals.h"
#include "GameplayTagContainer.h"

// Sets default values
ABPCombatCharacterBase::ABPCombatCharacterBase()
{
	bReplicates = false;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	AbilitySystemComponent->SetIsReplicated(false);

	CombatAttributeSet = CreateDefaultSubobject<UBPCombatAttributeSet>(TEXT("CombatAttributeSet"));

	AttackAbilityClass = UBPAttackGameplayAbility::StaticClass();

}

UAbilitySystemComponent* ABPCombatCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

// Called when the game starts or when spawned
void ABPCombatCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	ApplyInitialStats();
	GrantAttackAbility();

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UBPCombatAttributeSet::GetHealthAttribute()).AddUObject(this, &ABPCombatCharacterBase::HandleHealthChaged);


}

void ABPCombatCharacterBase::GrantAttackAbility()
{
	if (!AbilitySystemComponent || !AttackAbilityClass)
	{
		return;
	}

	if (!AbilitySystemComponent->FindAbilitySpecFromClass(AttackAbilityClass))
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AttackAbilityClass, 1));
	}
}

void ABPCombatCharacterBase::ApplyInitialStats()
{
	if (!AbilitySystemComponent || !InitialStatsEffect)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();

	EffectContext.AddSourceObject(this);

	const FGameplayEffectSpecHandle EffectSpec = AbilitySystemComponent->MakeOutgoingSpec(InitialStatsEffect, 1.0f, EffectContext);

	if (EffectSpec.IsValid())
	{
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());
	}
}

void ABPCombatCharacterBase::HandleHealthChaged(const FOnAttributeChangeData& Data)
{
	if (bIsDead || Data.NewValue > 0.0f)
	{
		return;
	}

	bIsDead = true;

	static const FGameplayTag AttackTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Ability.Attack")));
	static const FGameplayTag DeadTag = FGameplayTag::RequestGameplayTag(FName(TEXT("State.Dead")));

	AbilitySystemComponent->AddLooseGameplayTag(DeadTag);

	FGameplayTagContainer AttackTags(AttackTag);
	AbilitySystemComponent->CancelAbilities(&AttackTags);

	OnGasDeath();
}



// Called every frame
void ABPCombatCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABPCombatCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool ABPCombatCharacterBase::ApplyDamageEffectToTarget(AActor* TargetActor)
{
	if (!IsValid(TargetActor) || TargetActor == this)
	{
		return false;
	}

	if (!AbilitySystemComponent || !CombatAttributeSet || !DamageGameplayEffectClass)
	{
		return false;
	}

	UAbilitySystemComponent* TargetAbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);

	if (!TargetAbilitySystem)
	{
		return false;
	}

	static const FGameplayTag DeadTag = FGameplayTag::RequestGameplayTag(FName(TEXT("State.Dead")));

	if (AbilitySystemComponent->HasMatchingGameplayTag(DeadTag) || TargetAbilitySystem->HasMatchingGameplayTag(DeadTag))
	{
		return false;
	}

	const float DamageMagnitude = FMath::Max(CombatAttributeSet->GetAttackPower(), 0.0f);

	if (DamageMagnitude <= 0.0f)
	{
		return false;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();

	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle EffectSpec = AbilitySystemComponent->MakeOutgoingSpec(DamageGameplayEffectClass, 1.0f, EffectContext);

	if (!EffectSpec.IsValid())
	{
		return false;
	}

	static const FGameplayTag DamageDataTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Data.Damage")));

	EffectSpec.Data->SetSetByCallerMagnitude(DamageDataTag, DamageMagnitude);

	AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*EffectSpec.Data.Get(), TargetAbilitySystem);
	
	return true;
}

bool ABPCombatCharacterBase::TryActivateAttackAbility()
{
	if (!AbilitySystemComponent || !AttackAbilityClass || bIsDead)
	{
		return false;
	}

	return AbilitySystemComponent->TryActivateAbilityByClass(AttackAbilityClass);
}

