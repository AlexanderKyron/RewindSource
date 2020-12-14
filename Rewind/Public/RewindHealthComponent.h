

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RewindHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, URewindHealthComponent*, HealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnManaChangedSignature, URewindHealthComponent*, HealthComp, float, Mana, float, ManaDelta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REWIND_API URewindHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URewindHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY(BlueprintReadWrite, Category = "HealthComponent")
	float Health;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HealthComponent")
		float MaxHealth;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealthComponent")
		bool bIsDead;
	UPROPERTY(BlueprintReadWrite, Category = "HealthComponent")
		float Mana;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealthComponent")
		float MaxMana;

	UFUNCTION(BlueprintCallable)
		void AnyDamageHandler(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	UFUNCTION()
		void UseMana(float ManaAmnt);
	UFUNCTION()
		void Die();
	UFUNCTION(BlueprintCallable)
		void AddHealth(float HealthAmnt);
	UPROPERTY()
	float OldShields;
	


public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnManaChangedSignature OnManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnDeathSignature OnDeath;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealthComponent")
		float Shields;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealthComponent")
		float MaxShields;
};
