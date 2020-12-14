// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RewindCharacter.generated.h"
class ARewindWeapon;
class USpringArmComponent;
class UCameraComponent;
class UBoxComponent;
class URewindHealthComponent;
UENUM()
enum CombatMode
{
	Melee UMETA(DisplayName = "Melee"),
	Ranged UMETA(DisplayName = "Shooter"),
};
UCLASS(config=Game)
class ARewindCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ARewindCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		URewindHealthComponent* HealthComponent;

protected:

	virtual void BeginPlay() override;

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USpringArmComponent* CameraArm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
		float ShieldRegenRate;

	void BeginCrouch();
	void EndCrouch();
	void BeginZoom();
	void EndZoom();

	UFUNCTION(BlueprintCallable)
	void DisableFiring();

	UFUNCTION(BlueprintCallable)
	void EnableFiring();

	void StartSprinting();

	void StopSprinting();

	void Roll();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
	bool bZoom;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gameplay")
		float ZoomedFOV;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gameplay", meta = (ClampMin = 0.0, ClampMax = 100))
		float ZoomInterpSpeed;
	float DefaultFOV;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Gameplay")
		ARewindWeapon* ActiveWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Gameplay")
		TArray<ARewindWeapon*> WeaponInventory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Gameplay")
		TArray<TSubclassOf<ARewindWeapon>> WeaponInventory_Template;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
		UAnimMontage* RollForwardMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
		UAnimMontage* RollBackwardMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
		UAnimMontage* RollLeftMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
		UAnimMontage* RollRightMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
		UAnimMontage* RollForwardRMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
		UAnimMontage* RollForwardLMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
		UAnimMontage* RollBackwardRMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
		UAnimMontage* RollBackwardLMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* GrenadeThrowMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* WeaponSwitchMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
		int Grenades;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Movement")
		bool bCanRoll;
	
	UPROPERTY()
	bool bFiringEnabled;

	void Fire();
	void StopFire();
	void StartFire();

	float ForwardInput;
	float HorizontalInput;

	UPawnMovementComponent* MovementComponent;

	UCharacterMovementComponent* mComp;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void StopAnimation(UAnimMontage* Montage);

	UFUNCTION(BlueprintCallable)
	void SwitchWeapon(int WeaponIndex);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gameplay")
		int WeaponToSwitchTo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
		UBoxComponent* MeleeHitBox;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void StartGrenadeThrow();

	UFUNCTION(BlueprintNativeEvent, Category = "Weapon Functions")
		void PlayEffects_TimeSlow();
	UFUNCTION(BlueprintNativeEvent, Category = "Weapon Functions")
		void PlayEffects_TimeResume();

	UFUNCTION(BlueprintCallable)
	void ThrowGrenade();

	void SwitchFirstWeapon();

	void SwitchSecondWeapon();

	void SwitchThirdWeapon();

	UFUNCTION(BlueprintCallable)
	void AnimationCancel();

	UFUNCTION(BlueprintCallable)
	void StopReloads();

	UFUNCTION(BlueprintCallable)
	void EndDodge();

	UFUNCTION(BlueprintCallable)
	void EndMelee();

	UFUNCTION(BlueprintCallable)
		void DisableReloading();
	
	UFUNCTION(BlueprintCallable)
	void EnableReloading();

	void CancelGrenadeThrow();

	void Reload();

	virtual FVector GetPawnViewLocation() const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement)
		bool bIsSprinting = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		bool bCanSprint = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		float SprintSpeed = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		float MaxStamina = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement)
		float Stamina = MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		float StaminaRegenRate = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		float StaminaRegenDelay = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		float WalkSpeed = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		float StaminaDrainRate = 10;

	UPROPERTY()
		bool bCanMelee = true;

	UPROPERTY()
		bool bCanReload = true;

	UFUNCTION()
		void QuickMelee();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		TEnumAsByte<CombatMode> Mode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		int QuickMeleeComboCounter = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool ButtonWindow;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
		TArray<UAnimMontage*> ComboAttacks;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
		float MeleeDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
		bool bCanHitNext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
		int ActiveWeaponIndex;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
		bool bRolling;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
		bool bSlowTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
		float SlowMotionAmount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
		float PlayerSlowMotion;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
		bool bIsCutscene;
};

