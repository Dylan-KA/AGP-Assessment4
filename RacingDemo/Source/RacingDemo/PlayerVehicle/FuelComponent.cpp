// Fill out your copyright notice in the Description page of Project Settings.


#include "FuelComponent.h"
#include "WheeledVehiclePawn.h"

// Sets default values for this component's properties
UFuelComponent::UFuelComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	if (AWheeledVehiclePawn* WheeledVehiclePawn = Cast<AWheeledVehiclePawn>(GetOwner()))
	{
		MyVehicleMovementComponent = WheeledVehiclePawn->GetVehicleMovementComponent();
	}

}


// Called when the game starts
void UFuelComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


void UFuelComponent::ManageFuel(float DeltaTime)
{
	// Manage fuel system, update fuel amount,
	// if out of fuel then prevent vehicle from driving
	if (CurrentFuel <= 0.0f && !bIsOutOfFuel)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Out of fuel"))
		MyVehicleMovementComponent->SetHandbrakeInput(1.0f);
		bIsOutOfFuel = true;
		CurrentFuel = 0.0f;
	}
	if (bIsOutOfFuel)
	{
		MyVehicleMovementComponent->SetHandbrakeInput(1.0f);
	} else {
		UpdateFuelAmount(DeltaTime);
	}
}

// Called every frame
void UFuelComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ManageFuel(DeltaTime);
}

// If vehicle is driving then decrease amount of fuel
void UFuelComponent::UpdateFuelAmount(float DeltaTime)
{
	if (MyVehicleMovementComponent->GetForwardSpeedMPH() > 0.0f)
	{
		//UE_LOG(LogTemp, Warning, TEXT("CurrentFuel: %f"), CurrentFuel)
		CurrentFuel -= DeltaTime/10;
	}
}

void UFuelComponent::SetCurrentFuel(float NewFuel)
{
	CurrentFuel = NewFuel;
}

float UFuelComponent::GetCurrentFuel()
{
	return CurrentFuel;
}

bool UFuelComponent::IsOutOfFuel()
{
	return bIsOutOfFuel;
}

