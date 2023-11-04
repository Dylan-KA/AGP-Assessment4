// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ChaosVehicleMovementComponent.h"
#include "FuelComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RACINGDEMO_API UFuelComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFuelComponent();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Current fuel capacity, if out of fuel then cannot drive
	UPROPERTY()
	float CurrentFuel = 0.0f;
	UPROPERTY()
	bool bIsOutOfFuel = false;
	
	// ChaosVehicleMovementComponent used for getting current speed of vehicle
	UPROPERTY()
	UChaosVehicleMovementComponent* MyVehicleMovementComponent;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetCurrentFuel(float NewFuel);
	void AddFuel(float AddFuel);
	float GetCurrentFuel();

	// Returns true if out of fuel, false if not 
	UFUNCTION(BlueprintCallable)
	bool IsOutOfFuel();

	// If vehicle is driving then decrease amount of fuel
	void UpdateFuelAmount(float DeltaTime);
};
