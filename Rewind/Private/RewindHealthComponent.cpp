#include "RewindHealthComponent.h"
#include <Rewind\RewindCharacter.h>
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
URewindHealthComponent::URewindHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	MaxHealth = 100;
	Health = MaxHealth;
	MaxMana = 250;
	Mana = MaxMana;
	bIsDead = false;
	// ...
}
// Called when the game starts
void URewindHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	//Get the owner & bind the damage handler
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &URewindHealthComponent::AnyDamageHandler);
	}

}

/*
Damage handler.
Doesn't do status effects yet.

DamagedActor: Almost always the owner.
Damage: Amount of damage to process.
DamageType: Type of damage. Could denote status, crits, headshots.
InstigatedBy: The player that originated the command to do damage.
DamageCauser: The actor which sent the damage (ie: rifle)
*/
void URewindHealthComponent::AnyDamageHandler(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	float NextDamage = Damage;
	if (Shields > 0)
	{
		OldShields = Shields;
		Shields = FMath::Clamp(Shields - Damage, 0.0f, MaxHealth);
		if (Shields == 0) {
			NextDamage = FMath::Abs(OldShields - Damage);
		}
		else {
			NextDamage = 0;
		}
	}
	//Update health w/ clamping
	Health = FMath::Clamp(Health - NextDamage, 0.0f, MaxHealth);
	//Debug for health
	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(Health));
	//Run the blueprint event for the changing of health
	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
	//Killing the player
	if (Health <= 0.0f)
	{
		Die();
	}
}

/*
Function that changes the mana amount. Negative = plus mana, positive = less mana.
ManaAmnt is basically the mana delta to change by.
*/
void URewindHealthComponent::UseMana(float ManaAmnt)
{

	Mana = FMath::Clamp(Mana - ManaAmnt, 0.0f, MaxMana);
	OnManaChanged.Broadcast(this, Mana, ManaAmnt);
}

/*
Player death.
*/
void URewindHealthComponent::Die()
{
	
	bIsDead = true;
	OnDeath.Broadcast();
}

void URewindHealthComponent::AddHealth(float HealthAmnt)
{
	Health = FMath::Clamp(Health + HealthAmnt, 0.0f, MaxHealth);
}
