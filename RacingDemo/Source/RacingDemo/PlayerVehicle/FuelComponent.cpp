// Fill out your copyright notice in the Description page of Project Settings.


#include "FuelComponent.h"
#include "Net/UnrealNetwork.h"
#include "WheeledVehiclePawn.h"

// Sets default values for this component's properties
UFuelComponent::UFuelComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	if (AWheeledVehiclePawn* WheeledVehiclePawn = Cast<AWheeledVehiclePawn>(GetOwner()))
	{
		MyVehicleMovementComponent = WheeledVehiclePawn->GetVehicleMovementComponent();
	}

}

void UFuelComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UFuelComponent, CurrentFuel);
	DOREPLIFETIME(UFuelComponent, bIsOutOfFuel);
}

// Called when the game starts
void UFuelComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// If vehicle is driving then decrease amount of fuel
void UFuelComponent::UpdateFuelAmount(float DeltaTime)
{
	if (MyVehicleMovementComponent->GetForwardSpeedMPH() > 0.0f || MyVehicleMovementComponent->GetForwardSpeedMPH() < 0.0f)
	{
		//UE_LOG(LogTemp, Warning, TEXT("CurrentFuel: %f"), CurrentFuel)
		CurrentFuel -= DeltaTime/10;
	}
}

// Called every frame
void UFuelComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

// Sets the current fuel
void UFuelComponent::SetCurrentFuel(float NewFuel)
{
	CurrentFuel = NewFuel;
}

// Adds to the current fuel
void UFuelComponent::AddFuel(float NewFuel)
{
	CurrentFuel += NewFuel;
}

// Gets the current fuel amount
float UFuelComponent::GetCurrentFuel()
{
	return CurrentFuel;
}

void UFuelComponent::ClientAddFuel_Implementation(float NewFuel)
{
	AddFuel(NewFuel);
}

// Return true if out of fuel
bool UFuelComponent::IsOutOfFuel()
{
	if (CurrentFuel <= 0.0f)
	{
		return true;
	}
	return false;
}

