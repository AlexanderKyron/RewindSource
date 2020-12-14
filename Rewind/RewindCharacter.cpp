// Copyright Epic Games, Inc. All Rights Reserved.

#include "RewindCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Public/RewindWeapon.h"
#include "Public/RewindWeaponData.h"
#include "Public/RewindHealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Components/BoxComponent.h"

//////////////////////////////////////////////////////////////////////////
// ARewindCharacter

ARewindCharacter::ARewindCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	CameraArm->bUsePawnControlRotation = true;
	CameraArm->SetupAttachment(RootComponent);
	CameraArm->TargetArmLength = 220.0f;

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(CameraArm); 

	MeleeHitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("MeleeHitBox"));
	MeleeHitBox->SetupAttachment(RootComponent);

	HealthComponent = CreateDefaultSubobject<URewindHealthComponent>(TEXT("HealthComponent"));

	bCanRoll = true;
	bFiringEnabled = true;
}

void ARewindCharacter::BeginPlay() {
	Super::BeginPlay();
	DefaultFOV = CameraComp->FieldOfView;
	WeaponInventory.SetNum(WeaponInventory_Template.Num());
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	for (int i = 0; i < WeaponInventory_Template.Num(); i++) {
		TSubclassOf<ARewindWeapon> WeaponToSpawn = WeaponInventory_Template[i];
		ARewindWeapon* Weapon = Cast<ARewindWeapon>(GetWorld()->SpawnActor<AActor>(WeaponToSpawn, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams));
		if (Weapon) {
			Weapon->SetOwner(this);
			Weapon->bIsFiring = false;
			if (Weapon->WeaponData) {
				Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Weapon->HolsterSocket);
			}
			WeaponInventory[i] = Weapon;
		}

	}
	SwitchWeapon(0);
	MovementComponent = GetMovementComponent();
	mComp = Cast<UCharacterMovementComponent>(MovementComponent);

	WalkSpeed = mComp->MaxWalkSpeed;
}

void ARewindCharacter::PlayEffects_TimeSlow_Implementation() {

}
void ARewindCharacter::PlayEffects_TimeResume_Implementation() {

}
void ARewindCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
	ForwardInput = Value;

}

void ARewindCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
	HorizontalInput = Value;
}

void ARewindCharacter::BeginCrouch()
{
	if (!bIsCutscene) {
		if (bIsSprinting) {
			StopSprinting();
		}
		Crouch();
		if ((mComp->IsCrouching()))
			bIsCrouched = true;
	}
}

void ARewindCharacter::EndCrouch()
{
	UnCrouch();
	if (!(mComp->IsCrouching()))
		bIsCrouched = false;
}

void ARewindCharacter::BeginZoom()
{
	if (!bIsCutscene) {
		bZoom = true;
		if (bRolling && Stamina > 25) {
			bSlowTime = true;
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), SlowMotionAmount);
			this->CustomTimeDilation = PlayerSlowMotion;
			Stamina -= 25;
			PlayEffects_TimeSlow();
		}
		if (ActiveWeapon) {
			ActiveWeapon->Zoom();
		}
	}
}

void ARewindCharacter::EndZoom()
{
	bZoom = false;
	if (bSlowTime) {
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
		this->CustomTimeDilation = 1.0f;
		bSlowTime = false;
		PlayEffects_TimeResume();
	}
	if (ActiveWeapon) {
		ActiveWeapon->DeZoom();
	}
}
void ARewindCharacter::DisableFiring() {
	for (ARewindWeapon* Weapon : WeaponInventory) {
		Weapon->bCanShoot = false;
	}
	bFiringEnabled = false;
}
void ARewindCharacter::EnableFiring() {
	for (ARewindWeapon* Weapon : WeaponInventory) {
		Weapon->bCanShoot = true;
	}
	bFiringEnabled = true;
}

void ARewindCharacter::StartSprinting()
{
	if (!bIsCutscene) {
		if (mComp != nullptr && !bIsSprinting && Stamina > 20 && bCanSprint) {
			mComp->MaxWalkSpeed = SprintSpeed;
			bIsSprinting = true;
			if (bIsCrouched) {
				UnCrouch();
			}
			UE_LOG(LogTemp, Warning, TEXT("Sprinting Started | AFPSGameCharacter::StartSprinting()"));
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("mComp = nullptr | AFPSGameCharacter::StartSprinting()"));
		}
	}
}

void ARewindCharacter::StopSprinting()
{
	if (mComp != nullptr) {
		mComp->MaxWalkSpeed = WalkSpeed;
		bIsSprinting = false;
		UE_LOG(LogTemp, Warning, TEXT("Sprinting Ended | AFPSGameCharacter::StopSprinting()"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("mComp = nullptr | AFPSGameCharacter::StopSprinting()"));
	}
}
void ARewindCharacter::Roll()
{
	if (!bIsCutscene)
	{
		if (bCanRoll) {
			AnimationCancel();
			DisableFiring();
			bRolling = true;
			bCanMelee = false;
			//Roll right
			if (HorizontalInput == 1 && ForwardInput == 0) {
				GetMesh()->GetAnimInstance()->Montage_Play(RollRightMontage, 1.0f, EMontagePlayReturnType::Duration, 0.0f, true);
				bCanRoll = false;
			}
			//Roll left
			else if (HorizontalInput == -1 && ForwardInput == 0) {
				GetMesh()->GetAnimInstance()->Montage_Play(RollLeftMontage, 1.0f, EMontagePlayReturnType::Duration, 0.0f, true);
				bCanRoll = false;
			}
			//Roll Forward
			else if (HorizontalInput == 0 && ForwardInput == 1 || HorizontalInput == 0 && ForwardInput == 0) {
				GetMesh()->GetAnimInstance()->Montage_Play(RollForwardMontage, 1.0f, EMontagePlayReturnType::Duration, 0.0f, true);
				bCanRoll = false;
			}
			//Roll backward
			else if (HorizontalInput == 0 && ForwardInput == -1) {
				GetMesh()->GetAnimInstance()->Montage_Play(RollBackwardMontage, 1.0f, EMontagePlayReturnType::Duration, 0.0f, true);
				bCanRoll = false;
			}
			//Roll Forwards + Right
			else if (HorizontalInput == 1 && ForwardInput == 1) {
				GetMesh()->GetAnimInstance()->Montage_Play(RollForwardRMontage, 1.0f, EMontagePlayReturnType::Duration, 0.0f, true);
				bCanRoll = false;
			}
			//Roll Forwards + Left
			else if (HorizontalInput == -1 && ForwardInput == 1) {
				GetMesh()->GetAnimInstance()->Montage_Play(RollForwardLMontage, 1.0f, EMontagePlayReturnType::Duration, 0.0f, true);
				bCanRoll = false;
			}
			//Roll Backwards + Left
			else if (HorizontalInput == -1 && ForwardInput == -1) {
				GetMesh()->GetAnimInstance()->Montage_Play(RollBackwardLMontage, 1.0f, EMontagePlayReturnType::Duration, 0.0f, true);
				bCanRoll = false;
			}
			//Roll Backwards + Right
			else if (HorizontalInput == 1 && ForwardInput == -1) {
				GetMesh()->GetAnimInstance()->Montage_Play(RollBackwardRMontage, 1.0f, EMontagePlayReturnType::Duration, 0.0f, true);
				bCanRoll = false;
			}
		}
	}
}

void ARewindCharacter::StartFire()
{
	if (!bIsCutscene) {
		if (ActiveWeapon)
		{
			ActiveWeapon->BeginFire();
		}
	}
}
void ARewindCharacter::StopFire()
{
	if (ActiveWeapon)
	{
		ActiveWeapon->EndFire();
	}
}

// Called every frame
void ARewindCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	float TargetFOV = bZoom ? ZoomedFOV : DefaultFOV;
	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);
	CameraComp->SetFieldOfView(NewFOV);
	if (HealthComponent->Shields < HealthComponent->MaxShields) {
		HealthComponent->Shields += ShieldRegenRate * DeltaTime;
		HealthComponent->Shields = FMath::Clamp(HealthComponent->Shields, 0.0f, HealthComponent->MaxShields);
	}
	if (bSlowTime) {
		if (Stamina > 0) {
			Stamina -= StaminaDrainRate * DeltaTime;
		}
		else if (Stamina <= 0) {
			StopSprinting();
			if (bSlowTime) {
				UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
				this->CustomTimeDilation = 1.0f;
				bSlowTime = false;
					PlayEffects_TimeResume();
			}
		}
		if (Stamina < 0) {
			Stamina = 0;
		}
	}
	else {
		if (Stamina < MaxStamina) {
			Stamina += StaminaRegenRate * DeltaTime;
		}
		else if (Stamina > MaxStamina) {
			Stamina = MaxStamina;
		}
	}
}

void ARewindCharacter::StopAnimation(UAnimMontage* Montage)
{
	GetMesh()->GetAnimInstance()->Montage_Stop(0.0f, Montage);
}

void ARewindCharacter::SwitchWeapon(int WeaponIndex) {
	if (ActiveWeapon) {
		ActiveWeapon->CancelReload();
		if(ActiveWeapon->bIsFiring)
			ActiveWeapon->EndFire();
		ActiveWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, ActiveWeapon->HolsterSocket);
	}
	if (WeaponInventory.Num() > WeaponIndex && WeaponInventory[WeaponIndex] && WeaponInventory[WeaponIndex]->WeaponData != nullptr) {
		ActiveWeapon = WeaponInventory[WeaponIndex];
		ActiveWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "WeaponSocket");
		ZoomedFOV = DefaultFOV * (ActiveWeapon->WeaponData->ZoomMultiplier);
		ActiveWeaponIndex = WeaponIndex;
	}
	StopReloads();
}

// Called to bind functionality to input
void ARewindCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Base movement controlls on X and Y plane
	PlayerInputComponent->BindAxis("MoveForward", this, &ARewindCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARewindCharacter::MoveRight);

	//Mouselook controls
	PlayerInputComponent->BindAxis("LookUp", this, &ARewindCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ARewindCharacter::AddControllerYawInput);

	//Crouch implementations
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ARewindCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ARewindCharacter::EndCrouch);

	//Jump mapping
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ARewindCharacter::StopJumping);
	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ARewindCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ARewindCharacter::EndZoom);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ARewindCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ARewindCharacter::StopFire);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ARewindCharacter::Reload);

	PlayerInputComponent->BindAction("Roll", IE_Pressed, this, &ARewindCharacter::Roll);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ARewindCharacter::StartSprinting);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ARewindCharacter::StopSprinting);

	PlayerInputComponent->BindAction("Grenade", IE_Pressed, this, &ARewindCharacter::StartGrenadeThrow);
	PlayerInputComponent->BindAction("Grenade", IE_Released, this, &ARewindCharacter::ThrowGrenade);

	PlayerInputComponent->BindAction("Weapon1", IE_Pressed, this, &ARewindCharacter::SwitchFirstWeapon);
	PlayerInputComponent->BindAction("Weapon2", IE_Pressed, this, &ARewindCharacter::SwitchSecondWeapon);
	PlayerInputComponent->BindAction("Weapon3", IE_Pressed, this, &ARewindCharacter::SwitchThirdWeapon);

}

void ARewindCharacter::StartGrenadeThrow() {
	if (Grenades > 0) {
		//GetMesh()->GetAnimInstance()->Montage_Play(GrenadeThrowMontage, 1.0f, EMontagePlayReturnType::Duration, 0.4f, true);
		//DisableFiring();
		//DisableReloading();
		//bCanRoll = false;
	}
}

void ARewindCharacter::ThrowGrenade() {
	Grenades--;
}
void ARewindCharacter::SwitchFirstWeapon() {
	WeaponToSwitchTo = 0;
	GetMesh()->GetAnimInstance()->Montage_Play(WeaponSwitchMontage, 1.0f, EMontagePlayReturnType::Duration, 0.4f, true);
	//SwitchWeapon(0);
}
void ARewindCharacter::SwitchSecondWeapon() {
	WeaponToSwitchTo = 1;
	GetMesh()->GetAnimInstance()->Montage_Play(WeaponSwitchMontage, 1.0f, EMontagePlayReturnType::Duration, 0.4f, true);
	//SwitchWeapon(1);

}
void ARewindCharacter::SwitchThirdWeapon() {
	WeaponToSwitchTo = 2;
	GetMesh()->GetAnimInstance()->Montage_Play(WeaponSwitchMontage, 1.0f, EMontagePlayReturnType::Duration, 0.4f, true);
	//SwitchWeapon(2);
}

void ARewindCharacter::AnimationCancel() {
	GetMesh()->GetAnimInstance()->Montage_Stop(0.f);
	StopReloads();
	EnableFiring();
	EndMelee();
	CancelGrenadeThrow();
}

void ARewindCharacter::StopReloads() {
	for (ARewindWeapon* Weapon : WeaponInventory) {
		if (Weapon != nullptr) {
			Weapon->bReloading = false;
			Weapon->bCanShoot = true;
			if(Weapon->GunMesh != nullptr && Weapon->GunMesh->GetAnimInstance())
				Weapon->GunMesh->GetAnimInstance()->Montage_Stop(0.0f);
			if(Weapon->WeaponData)
				GetMesh()->GetAnimInstance()->Montage_Stop(0.0f, Weapon->WeaponData->PlayerReloadMontage);
		}
	}
}

void ARewindCharacter::EndDodge() {
	bCanRoll = true;
	bRolling = false;
	EnableFiring();
	//StopReloads();
	bCanMelee = true;
	EnableReloading();
}

void ARewindCharacter::EndMelee() {
	EnableReloading();
	EnableFiring();
	bCanHitNext = true;
	bCanRoll = true;
	bCanMelee = true;
}

void ARewindCharacter::QuickMelee() {
	DisableReloading();
	DisableFiring();
	
}

void ARewindCharacter::DisableReloading() {
	bCanReload = false;
}

void ARewindCharacter::EnableReloading() {
	bCanReload = true;
}
void ARewindCharacter::CancelGrenadeThrow() {
	GetMesh()->GetAnimInstance()->Montage_Stop(0.0f, GrenadeThrowMontage);
	EnableFiring();
	EnableReloading();
}
				

void ARewindCharacter::Reload() {
	if (ActiveWeapon && bCanReload) {
		ActiveWeapon->BeginReload();
	}
}

FVector ARewindCharacter::GetPawnViewLocation() const
{
	if (CameraComp)
	{
		return  CameraComp->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}

void ARewindCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARewindCharacter, ActiveWeapon);
}