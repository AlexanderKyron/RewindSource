// Fill out your copyright notice in the Description page of Project Settings.


#include "RespawnGrenade.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ARespawnGrenade::ARespawnGrenade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARespawnGrenade::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(TimerHandle_Explode, this, &ARespawnGrenade::ExplodeGrenade, 5.0f, false, 0.0f);
}

// Called every frame
void ARespawnGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARespawnGrenade::ExplodeGrenade() {
	//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionSystem, GetWorldTransform(), true, EPSCPoolMethod::AutoRelease,true);
}