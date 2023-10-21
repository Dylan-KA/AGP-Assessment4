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

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Current fuel capacity, if out of fuel then cannot drive
	float CurrentFuel = 0.0f;
	bool bIsOutOfFuel = false;

	// ChaosVehicleMovementComponent used for getting current speed of vehicle
	UPROPERTY()
	UChaosVehicleMovementComponent* MyVehicleMovementComponent;

	// Called during tick function to handle empty fuel
	void ManageFuel(float DeltaTime);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetCurrentFuel(float NewFuel);
	float GetCurrentFuel();
	bool IsOutOfFuel();

	// If vehicle is driving then decrease amount of fuel
	void UpdateFuelAmount(float DeltaTime);
};
