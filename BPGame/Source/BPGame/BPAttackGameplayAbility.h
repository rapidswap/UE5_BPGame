#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "BPAttackGameplayAbility.generated.h"

class UAnimMontage;

/**
 * 공격 몽타주가 끝날 때까지 State.Attacking 태그를 유지하는 공통 공격 어빌리티.
 * 플레이어와 몬스터는 이 클래스를 부모로 하는 Gameplay Ability BP를 만들고
 * AttackMontage만 각각 지정하면 된다.
 */
UCLASS(Blueprintable)
class BPGAME_API UBPAttackGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UBPAttackGameplayAbility();

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attack")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attack", meta=(ClampMin="0.01"))
	float PlayRate = 1.0f;

private:
	UFUNCTION()
	void HandleMontageCompleted();

	UFUNCTION()
	void HandleMontageCancelled();

	void FinishAbility(bool bWasCancelled);
};
