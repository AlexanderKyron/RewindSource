// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RewindWeapon.generated.h"

class URewindWeaponData;
class USceneComponent;
class USkeletalMeshComponent;
class ARewindCharacter;
class ADamagePopup;
UCLASS()
class REWIND_API ARewindWeapon : public AActor
{
	GENERATED_BODY()
	
public:

	UPROPERTY(VisibleDefaultsOnly, Category = Gameplay)
	ARewindCharacter* PlayerCharacter;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		URewindWeaponData* WeaponData;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Mesh)
		USkeletalMeshComponent* GunMesh;
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USceneComponent* MuzzleComp;
public:	
	// Sets default values for this actor's properties
	ARewindWeapon();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		FName HolsterSocket;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
		bool bCanShoot;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void Fire();
	void SpawnFireEffects();
	void SpawnImpactEffects(FHitResult HitData);

	UFUNCTION(BlueprintCallable)
	void Reload();

	
	UFUNCTION(BlueprintNativeEvent, Category = "Weapon Functions")
		void SpawnEffectsBP(FHitResult HitData);
	UFUNCTION(BlueprintNativeEvent, Category = "Weapon Functions")
		void PlayReloadAnimation();
	UFUNCTION(BlueprintNativeEvent, Category = "Weapon Functions")
		void EndReloadAnimation();
	UFUNCTION()
		void DealDamage(AActor* HitActor, FHitResult HitData, AActor* WeaponOwner, FVector ShotDirection, EPhysicalSurface SurfaceType, FVector HitLocation);
	
	
public:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
		bool bReloading;
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY()
	bool bIsFiring;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
		TSubclassOf<ADamagePopup> DamagePopup;

	void BeginFire();
	void EndFire();
	void BeginReload();
	void CancelReload();
	void Zoom();
	void DeZoom();
};
