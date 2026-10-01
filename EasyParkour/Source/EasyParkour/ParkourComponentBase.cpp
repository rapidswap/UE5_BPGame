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
			OutReversedNormal = -HitResult.Normal.GetSafeNormal();
			return true;
		}
	}

	return false;
}

bool UParkourComponentBase::ScanWallNative(const FVector& DetectLocation, const FVector& ReversedNormal, FParkourWallScanResult& OutResult) const
{
	OutResult = FParkourWallScanResult();

	if (!IsValid(OwnerCharacter) || ReversedNormal.IsNearlyZero())
	{
		return false;
	}

	const FVector InitialWallDirection = ReversedNormal.GetSafeNormal();
	const ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_Visibility);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwnerCharacter.Get());

	FHitResult FacedWallHit;

	for (int32 Index = 0; Index < 30; ++Index)
	{
		const FVector TraceBase = DetectLocation + FVector::UpVector * 300.0f - FVector::UpVector * (Index * 10.0f);

		const FVector TraceStart = TraceBase - InitialWallDirection * 20.0f;

		const FVector TraceEnd = TraceBase + InitialWallDirection * 80.0f;

		const bool bFacedWallHit = UKismetSystemLibrary::LineTraceSingle(
			this,
			TraceStart,
			TraceEnd,
			TraceChannel,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::ForDuration,
			FacedWallHit,
			true
		);
		
		if (bFacedWallHit)
		{
			OutResult.bHasFacedWall = true;
			OutResult.FacedWallHitResult = FacedWallHit;
			break;
		}
	}

	if (!OutResult.bHasFacedWall)
	{ 
		return false;
	}

	const FVector WallDirection = -OutResult.FacedWallHitResult.Normal.GetSafeNormal();

	OutResult.WallRotation = WallDirection.Rotation();

	bool bReachedTopEnd = false;

	for (int32 Index = 0; Index < 10; ++Index)
	{
		const FVector TopBase = OutResult.FacedWallHitResult.Location + WallDirection * (Index * 20.0f);

		const FVector TraceStart = TopBase + FVector::UpVector * 10.0f;

		const FVector TraceEnd = TopBase;

		FHitResult TopHit;

		const bool bTopHit = UKismetSystemLibrary::SphereTraceSingle(
			this,
			TraceStart,
			TraceEnd,
			5.0f,
			TraceChannel,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::ForDuration,
			TopHit,
			true
		);

		if (Index == 0)
		{
			if (!bTopHit)
			{
				return false;
			}

			OutResult.bHasFirstTop = true;
			OutResult.FirstTopHitResult = TopHit;

			// 첫 지점 하나만 감지된 짧은 벽도 처리할 수 있도록 초기화.
			OutResult.bHasLastTop = true;
			OutResult.LastTopHitResult = TopHit;
			continue;
			
		}

		if (bTopHit)
		{
			OutResult.bHasLastTop = true;
			OutResult.LastTopHitResult = TopHit;
			continue;
		}

		// 여기 도달하면 이전 지점과 현재 지점사이에 벽 끝이 있음.
		bReachedTopEnd = true;
		break;
	}

	// 상단을 모두 검사해도 끊기는 지점을 발견하지 못했다면,
	// 기본 벽 스캔은 성공으로 처리하고 벽 끝/착지 결과는 비워 둔다.
	if (!bReachedTopEnd)
	{
		return true;
	}

	// 벽 끝 검사.
	const FVector EdgeEnd = OutResult.LastTopHitResult.ImpactPoint;
	const FVector EdgeStart = EdgeEnd + WallDirection * 20.0f;

	FHitResult EdgeHit;
	const bool bEdgeHit = UKismetSystemLibrary::SphereTraceSingle(
		this,
		EdgeStart,
		EdgeEnd,
		10.0f,
		TraceChannel,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		EdgeHit,
		true
	);

	if (!bEdgeHit)
	{
		return true;
	}

	OutResult.bHasEndOfWall = true;
	OutResult.EndOfWallHitResult = EdgeHit;

	// 착지 위치 검사.
	const FVector LandingStart = EdgeHit.ImpactPoint + WallDirection * 60.0f;
	const FVector LandingEnd = LandingStart - FVector::UpVector * 180.0f;

	FHitResult LandingHit;
	const bool bLandingHit = UKismetSystemLibrary::SphereTraceSingle(
		this,
		LandingStart,
		LandingEnd,
		10.0f,
		TraceChannel,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		LandingHit,
		true
	);

	if (bLandingHit)
	{
		OutResult.bHasVaultLanding = true;
		OutResult.VaultLandingResult = LandingHit;
	}

	return OutResult.bHasFacedWall && OutResult.bHasFirstTop;
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

