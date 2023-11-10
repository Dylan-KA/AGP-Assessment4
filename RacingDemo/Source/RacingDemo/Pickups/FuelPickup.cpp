// Fill out your copyright notice in the Description page of Project Settings.


#include "FuelPickup.h"
#include "RacingDemo/PlayerVehicle/PCGVehiclePawn.h"

void AFuelPickup::OnPickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitInfo)
{
	APCGVehiclePawn* VehiclePawn = Cast<APCGVehiclePawn>(OtherActor);
	if(VehiclePawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("FuelPickup collided with VehiclePawn with LocalRole: %d"), GetLocalRole())

		// Adds fuel to the vehicle then removes pickup.
		UFuelComponent* FuelComponent = VehiclePawn->GetComponentByClass<UFuelComponent>();
		if(FuelComponent)
		{
			FuelComponent->AddFuel(FuelAmount);
		}
		if (GetNetMode() == NM_ListenServer || GetNetMode() == NM_DedicatedServer)
		{
			if(FuelComponent)
			{
				// RPC call to clients
				FuelComponent->ClientAddFuel(FuelAmount);
			}
			
		}
		Destroy();
	}
	

}
