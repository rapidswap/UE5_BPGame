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


USTRUCT(BlueprintType)
struct EASYPARKOUR_API FParkourWallScanResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool bHasFacedWall = false;

	UPROPERTY(BlueprintReadOnly)
	bool bHasFirstTop = false;

	UPROPERTY(BlueprintReadOnly)
	bool bHasLastTop = false;

	UPROPERTY(BlueprintReadOnly)
	bool bHasEndOfWall = false;

	UPROPERTY(BlueprintReadOnly)
	bool bHasVaultLanding = false;
	
	UPROPERTY(BlueprintReadOnly)
	FHitResult FacedWallHitResult;

	UPROPERTY(BlueprintReadOnly)
	FHitResult FirstTopHitResult;

	UPROPERTY(BlueprintReadOnly)
	FHitResult LastTopHitResult;

	UPROPERTY(BlueprintReadOnly)
	FHitResult EndOfWallHitResult;

	UPROPERTY(BlueprintReadOnly)
	FHitResult VaultLandingResult;

	UPROPERTY(BlueprintReadOnly)
	FRotator WallRotation = FRotator::ZeroRotator;
};

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
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Parkour|Detection", meta = (DisplayName = "Scan Wall Native"))
	bool ScanWallNative(const FVector& DetectLocation, const FVector& ReversedNormal, FParkourWallScanResult& OutResult) const;


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
