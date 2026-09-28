// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/HitResult.h"
#include "ParkourComponentBase.generated.h"

// 전방 선언.
class ACharacter;
class UCapsuleComponent;
class UCharacterMovementComponent;
class USkeletalMeshComponent;


UCLASS( Blueprintable, BlueprintType,ClassGroup=(Parkour), meta=(BlueprintSpawnableComponent) )
class EASYPARKOUR_API UParkourComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UParkourComponentBase();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Parkour|Detection", meta = (DisplayName = "Detect Wall Native"))
	bool DetectWallNative(FHitResult& OutHitResult, FVector& OutHitLocation, FVector& OutReversedNormal) const;
	
	UFUNCTION(BlueprintCallable,BlueprintPure=false,Category="Pakour|Detection",meta = (DisplayName = "Scan Wall Native"))
	void ScanWallNative() const;


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


private:
	bool InitializeOwnerReferences();

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Parkour|References", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ACharacter> OwnerCharacter;
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Parkour|References", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> OwnerMesh;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Parkour|References", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> OwnerCapsule;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Parkour|References", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCharacterMovementComponent> OwnerMovement;
};
