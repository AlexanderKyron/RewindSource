// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RewindWeaponData.generated.h"

class USkeletalMeshComponent;
class UParticleSystem;
class UDamageType;
USTRUCT()
struct FHitScanTrace {
	GENERATED_BODY()
public: 
	UPROPERTY()
		FVector_NetQuantize TraceFrom;
	UPROPERTY()
		TEnumAsByte<EPhysicalSurface> SurfaceType;
	UPROPERTY()
		FVector_NetQuantize TraceTo;
};
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REWIND_API URewindWeaponData : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URewindWeaponData();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
		float Damage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
		float HeadshotMultiplier;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
		float CritMultiplier;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
		float StatusChance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
		float CritChance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
		float StatusDamage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
		TSubclassOf<UDamageType> StatusType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
		TSubclassOf<UDamageType> BaseDamageType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
		TSubclassOf<UDamageType> CritDamageType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
		TSubclassOf<UDamageType> VulnDamageType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
		TSubclassOf<UDamageType> VulnCrit;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
		float VulnMultiplier;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
		TSubclassOf<UCameraShake> FireCamShake;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
		float SpreadDegrees;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
		float ZoomSpread;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
		float HipSpread;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
		float Range;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
		bool bIsFullAuto;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
		float FireRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
		int Multishot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		bool bDoesUseMagazines;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		int MagSize;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		int BulletsInMag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		int MaxAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		int Ammo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
		UParticleSystem* MuzzleEffect;
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	//	AActor* EffectSpawner;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
		UParticleSystem* TracerEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
		FName TracerTarget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
		FName MuzzleTarget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
		float ZoomMultiplier;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
		float ZoomInterpSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
		float MoveSpeedMultiplier;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
		float ReloadTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
		FName MuzzleSocketName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
		UAnimMontage* WeaponFireMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* PlayerReloadMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* WeaponReloadMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	FVector TracerEndPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float LastFireTime;
	

	UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
		FHitScanTrace HitScanTrace;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FTimerHandle TimerHandle_TimeBetweenShots;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FTimerHandle TimerHandle_Reload;
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
