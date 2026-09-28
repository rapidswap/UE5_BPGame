// Fill out your copyright notice in the Description page of Project Settings.


#include "ParkourComponentBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UParkourComponentBase::UParkourComponentBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UParkourComponentBase::BeginPlay()
{
	Super::BeginPlay();

	InitializeOwnerReferences();
	
}

bool UParkourComponentBase::DetectWallNative(FHitResult& OutHitResult, FVector& OutHitLocation, FVector& OutReversedNormal) const
{
	OutHitResult = FHitResult();
	OutHitLocation = FVector::ZeroVector;
	OutReversedNormal = FVector::ZeroVector;

	if (!IsValid(OwnerCharacter))
	{
		UE_LOG(LogTemp, Log, TEXT("DetectWallNative Failed: OwnerCharacter is invalid."));
		return false;
	}
	
	const FVector ActorLocation = OwnerCharacter->GetActorLocation();
	const FVector ForwardVector = OwnerCharacter->GetActorForwardVector();

	constexpr int32 TraceCount = 8;
	constexpr float InitialDownOffset = 60.0f;
	constexpr float HeightStep = 20.0f;
	constexpr float BackwardOffset = 30.0f;
	constexpr float ForwardDistance = 200.0f;
	constexpr float TraceRadius = 10.0f;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwnerCharacter.Get());

	const ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_Visibility);

	for (int32 Index = 0; Index < TraceCount;++Index)
	{
		const float HeightOffset = -InitialDownOffset + (Index * HeightStep);

		const FVector TraceBaseLocation = ActorLocation + FVector::UpVector * HeightOffset;

		const FVector TraceStart = TraceBaseLocation - ForwardVector * BackwardOffset;

		const FVector TraceEnd = TraceBaseLocation + ForwardVector * ForwardDistance;

		FHitResult HitResult;

		const bool bHit = UKismetSystemLibrary::SphereTraceSingle(
			this,
			TraceStart,
			TraceEnd,
			TraceRadius,
			TraceChannel,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::ForOneFrame,
			HitResult,
			true
		);

		if (bHit)
		{
			OutHitResult = HitResult;
			OutHitLocation = HitResult.Location;
			OutReversedNormal = -HitResult.ImpactNormal.GetSafeNormal();
			return true;
		}
	}

	return false;
}

void UParkourComponentBase::ScanWallNative() const
{
}


// Called every frame
void UParkourComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UParkourComponentBase::InitializeOwnerReferences()
{
	OwnerCharacter = Cast<ACharacter>(GetOwner());

	if (!IsValid(OwnerCharacter))
	{
		UE_LOG(LogTemp, Log, TEXT("ParkourComponentBase requires an ACharacter owner. Onwer: %s"), *GetNameSafe(GetOwner()));
		return false;
	}

	OwnerMesh = OwnerCharacter->GetMesh();
	OwnerCapsule = OwnerCharacter->GetCapsuleComponent();
	OwnerMovement = OwnerCharacter->GetCharacterMovement();

	const bool bInitialized = IsValid(OwnerMesh) && IsValid(OwnerCapsule) && IsValid(OwnerMovement);

	if (!bInitialized)
	{
		UE_LOG(LogTemp, Log, TEXT("ParkourComponentBase failed to cache components for %s"), *GetNameSafe(OwnerCharacter));

		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("ParkourComponentBase initialized for %s"), *GetNameSafe(OwnerCharacter));

	return true;
}

