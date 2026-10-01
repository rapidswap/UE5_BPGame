#include "BPAttackGameplayAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequenceBase.h"
#include "BPCombatCharacterBase.h"
#include "NativeGameplayTags.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Ability_Attack, "Ability.Attack");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_State_Attacking, "State.Attacking");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_State_Dead, "State.Dead");

UBPAttackGameplayAbility::UBPAttackGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;

	FGameplayTagContainer AssetTags;
	AssetTags.AddTag(TAG_Ability_Attack);
	SetAssetTags(AssetTags);

	ActivationOwnedTags.AddTag(TAG_State_Attacking);
	ActivationBlockedTags.AddTag(TAG_State_Attacking);
	ActivationBlockedTags.AddTag(TAG_State_Dead);
}

void UBPAttackGameplayAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	UAnimMontage* MontageToPlay = AttackMontage;

	if (!MontageToPlay)
	{
		const ABPCombatCharacterBase* CombatCharacter =
			ActorInfo ? Cast<ABPCombatCharacterBase>(ActorInfo->AvatarActor.Get()) : nullptr;

		if (CombatCharacter)
		{
			UAnimSequenceBase* AttackAnimation = CombatCharacter->GetAttackAnimation();
			MontageToPlay = Cast<UAnimMontage>(AttackAnimation);

			if (!MontageToPlay && AttackAnimation)
			{
				MontageToPlay = UAnimMontage::CreateSlotAnimationAsDynamicMontage(
					AttackAnimation,
					CombatCharacter->GetAttackSlotName(),
					0.1f,
					0.1f);
			}
		}
	}

	if (!MontageToPlay || !CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}

	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			MontageToPlay,
			PlayRate);

	MontageTask->OnCompleted.AddDynamic(this, &UBPAttackGameplayAbility::HandleMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UBPAttackGameplayAbility::HandleMontageCancelled);
	MontageTask->OnCancelled.AddDynamic(this, &UBPAttackGameplayAbility::HandleMontageCancelled);
	MontageTask->ReadyForActivation();

	if (ABPCombatCharacterBase* CombatCharacter = Cast<ABPCombatCharacterBase>(ActorInfo->AvatarActor.Get()))
	{
		CombatCharacter->OnGasAttackStarted();
	}
}

void UBPAttackGameplayAbility::HandleMontageCompleted()
{
	FinishAbility(false);
}

void UBPAttackGameplayAbility::HandleMontageCancelled()
{
	FinishAbility(true);
}

void UBPAttackGameplayAbility::FinishAbility(const bool bWasCancelled)
{
	if (!IsActive())
	{
		return;
	}

	if (ABPCombatCharacterBase* CombatCharacter = Cast<ABPCombatCharacterBase>(GetAvatarActorFromActorInfo()))
	{
		CombatCharacter->OnGasAttackEnded(bWasCancelled);
	}

	EndAbility(
		GetCurrentAbilitySpecHandle(),
		GetCurrentActorInfo(),
		GetCurrentActivationInfo(),
		false,
		bWasCancelled);
}
