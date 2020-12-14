// Fill out your copyright notice in the Description page of Project Settings.


#include "CollisionQueryParams.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "RewindWeapon.h"
#include "RewindWeaponData.h"
#include "RewindHealthComponent.h"
#include "DamagePopup.h"
#include <Rewind\Rewind.h>
#include <Rewind\RewindCharacter.h>


// Sets default values
ARewindWeapon::ARewindWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true; 
	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	RootComponent = GunMesh;
	GunMesh->bCastDynamicShadow = false;
	GunMesh->CastShadow = false;
	MuzzleComp = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleComp"));
	MuzzleComp->SetupAttachment(GunMesh);
	WeaponData = CreateDefaultSubobject<URewindWeaponData>(TEXT("WeaponData"));
	WeaponData->TracerTarget = "Target";
	WeaponData->MuzzleTarget = "Muzzle";
	WeaponData->ZoomMultiplier = 0.9f;
	WeaponData->ZoomInterpSpeed = 20.0f;
	bIsFiring = false;
	bCanShoot = true;

}
void ARewindWeapon::BeginPlay()
{
	Super::BeginPlay();
	//PlayerCharacter = Cast<ARewindCharacter>(GetOwner());
	bIsFiring = false;
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay:RewindWeapon.cpp"));
	PlayerCharacter = Cast<ARewindCharacter>(GetOwner());
	
}
void ARewindWeapon::Fire()
{
	if (bCanShoot) {
		if (((WeaponData->bDoesUseMagazines == true && WeaponData->BulletsInMag > 0) || (WeaponData->bDoesUseMagazines == false && WeaponData->Ammo > 0))) {
			if (WeaponData->WeaponFireMontage) {
				GunMesh->GetAnimInstance()->Montage_Play(WeaponData->WeaponFireMontage, 1.0f, EMontagePlayReturnType::Duration, 0.0f, true);
			}
			for (int i = 0; i < WeaponData->Multishot; i++) {
				UE_LOG(LogTemp, Warning, TEXT("Firing a pellet"));
				AActor* WeaponOwner = GetOwner();
				if (!WeaponOwner) {
					return;
				}
				FVector EyeLocation;
				FRotator EyeRotation;
				WeaponOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

				FVector ShotDirection = EyeRotation.Vector();

				FVector TraceEnd = EyeLocation + FMath::VRandCone(ShotDirection, WeaponData->SpreadDegrees) * WeaponData->Range;
				FCollisionQueryParams QueryParams;
				QueryParams.AddIgnoredActor(WeaponOwner);
				QueryParams.AddIgnoredActor(this);
				QueryParams.bTraceComplex = true;
				QueryParams.bReturnPhysicalMaterial = true;

				WeaponData->TracerEndPoint = TraceEnd;
				EPhysicalSurface SurfaceType = SurfaceType_Default;
				FHitResult HitData;
				if (GetWorld()->LineTraceSingleByChannel(HitData, EyeLocation, TraceEnd, ECC_Visibility, QueryParams))
				{
					AActor* HitActor = HitData.GetActor();

					SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitData.PhysMaterial.Get());
					DealDamage(HitActor, HitData, WeaponOwner, ShotDirection, SurfaceType, HitData.ImpactPoint);
					SpawnImpactEffects(HitData);
					WeaponData->TracerEndPoint = HitData.ImpactPoint;
				}
				UE_LOG(LogTemp, Warning, TEXT("Spawning fire effects"));
				SpawnFireEffects();
			}
			WeaponData->BulletsInMag--;
			WeaponData->LastFireTime = GetWorld()->TimeSeconds;
		}
		else {
			BeginReload();
		}
	}
}

void ARewindWeapon::SpawnFireEffects()
{
	if (WeaponData->MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(WeaponData->MuzzleEffect, GunMesh, WeaponData->MuzzleSocketName);
		UE_LOG(LogTemp, Warning, TEXT("Spawned muzzle effect"))
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No muzzle effect!"))

	}
	if (WeaponData->TracerEffect)
	{
		FVector MuzzleLocation = GunMesh->GetSocketLocation(WeaponData->MuzzleSocketName);
		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponData->TracerEffect, MuzzleLocation);
		if (TracerComp)
		{
			TracerComp->SetVectorParameter(WeaponData->TracerTarget, WeaponData->TracerEndPoint);
		}

	}

	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			PC->ClientPlayCameraShake(WeaponData->FireCamShake);
			UE_LOG(LogTemp, Warning, TEXT("Shake played"));
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Failed to cast PC for shake"));
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Could not get owner!"));
	}
}

void ARewindWeapon::SpawnImpactEffects(FHitResult HitData)
{
	SpawnEffectsBP(HitData);
}


void ARewindWeapon::PlayReloadAnimation_Implementation()
{
}
void ARewindWeapon::EndReloadAnimation_Implementation()
{
}

void ARewindWeapon::DealDamage(AActor* HitActor, FHitResult HitData, AActor* WeaponOwner, FVector ShotDirection, EPhysicalSurface SurfaceType, FVector HitLocation)
{
	bool bStatusHit = (FMath::RandRange(0, 100) <= WeaponData->StatusChance);
	bool bCritHit = (FMath::RandRange(0, 100) <= WeaponData->CritChance);
	TSubclassOf<UDamageType> DamageToApply = WeaponData->BaseDamageType;
	float DamageToDeal = WeaponData->Damage;
	if (bCritHit) {
		DamageToDeal *= WeaponData->CritMultiplier;
		DamageToApply = WeaponData->CritDamageType;
	} if (SurfaceType == SURFACE_WEAKFLESH) {
		DamageToDeal *= WeaponData->VulnMultiplier;
		DamageToApply = WeaponData->VulnDamageType;
	}
	UGameplayStatics::ApplyPointDamage(HitActor, DamageToDeal, ShotDirection, HitData, WeaponOwner->GetInstigatorController(), this, DamageToApply);
	if (bStatusHit) {
		UGameplayStatics::ApplyPointDamage(HitActor, DamageToDeal + WeaponData->StatusDamage, ShotDirection, HitData, WeaponOwner->GetInstigatorController(), this, WeaponData->StatusType);
	}
	URewindHealthComponent* EnemyHealth = HitActor->FindComponentByClass<URewindHealthComponent>();
	if (EnemyHealth != nullptr) {
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ADamagePopup* DamagePop = Cast<ADamagePopup>(GetWorld()->SpawnActor<AActor>(DamagePopup, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams));
			DamagePop->DamageAmount = DamageToDeal;
		DamagePop->bIsCrit = bCritHit;
		DamagePop-> bIsStatus = bStatusHit;
		DamagePop->pop();
	}
}

// Called every frame
void ARewindWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARewindWeapon::BeginFire()
{
	float FirstDelay = FMath::Max(WeaponData->LastFireTime + WeaponData->FireRate - GetWorld()->TimeSeconds, 0.0f);
	GetWorldTimerManager().SetTimer(WeaponData->TimerHandle_TimeBetweenShots, this, &ARewindWeapon::Fire, WeaponData->FireRate, WeaponData->bIsFullAuto, FirstDelay);
	bIsFiring = true;
}

void ARewindWeapon::EndFire()
{
	bIsFiring = false;
	GetWorldTimerManager().ClearTimer(WeaponData->TimerHandle_TimeBetweenShots);
}

void ARewindWeapon::BeginReload()
{
	UE_LOG(LogTemp, Warning, TEXT("BeginReload()"));
	PlayerCharacter = Cast<ARewindCharacter>(GetOwner());
	if (WeaponData->bDoesUseMagazines && WeaponData->BulletsInMag < WeaponData->MagSize && bReloading == false && PlayerCharacter->bCanReload) {
		bReloading = true;
		bCanShoot = false;
		EndFire();
		PlayerCharacter = Cast<ARewindCharacter>(GetOwner());
		PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(WeaponData->PlayerReloadMontage, 1.0f, EMontagePlayReturnType::Duration, 0.0f, true);
		UE_LOG(LogTemp, Warning, TEXT("Playing character reload"));
		GunMesh->GetAnimInstance()->Montage_Play(WeaponData->WeaponReloadMontage, 1.0f, EMontagePlayReturnType::Duration, 0.0f, true);
		UE_LOG(LogTemp, Warning, TEXT("Playing Weapon reload"));

//		GetWorldTimerManager().SetTimer(WeaponData->TimerHandle_Reload, this, &ARewindWeapon::Reload, WeaponData->ReloadTime, false, WeaponData->ReloadTime);
	}
}

void ARewindWeapon::Reload()
{
	if (/*WeaponData->Ammo - (WeaponData->MagSize - WeaponData->BulletsInMag) > 0 &&*/ WeaponData->bDoesUseMagazines)
	{
		//WeaponData->Ammo -= (WeaponData->MagSize - WeaponData->BulletsInMag);
		WeaponData->BulletsInMag = WeaponData->MagSize;
	}
//	GetWorldTimerManager().ClearTimer(WeaponData->TimerHandle_Reload);
	bReloading = false;
	bCanShoot = true;
}

void ARewindWeapon::SpawnEffectsBP_Implementation(FHitResult HitData)
{
}

void ARewindWeapon::CancelReload()
{
	bReloading = false;
	bCanShoot = true;
    GunMesh->GetAnimInstance()->Montage_Stop(0.0f);
}
void ARewindWeapon::Zoom() {
	WeaponData->SpreadDegrees = WeaponData->ZoomSpread;
}
void ARewindWeapon::DeZoom() {
	WeaponData->SpreadDegrees = WeaponData->HipSpread;
}
