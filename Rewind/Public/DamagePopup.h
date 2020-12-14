// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamagePopup.generated.h"

UCLASS()
class REWIND_API ADamagePopup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADamagePopup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(BlueprintReadWrite, Category = Damage)
		float DamageAmount;
	UPROPERTY(BlueprintReadWrite, Category = Damage)
		bool bIsCrit;
	UPROPERTY(BlueprintReadWrite, Category = Damage)
		bool bIsStatus;
	UFUNCTION(BlueprintNativeEvent)
		void pop();
};
