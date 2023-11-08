// Fill out your copyright notice in the Description page of Project Settings.


#include "FuelPickup.h"

#include "RacingDemo/PlayerVehicle/PCGVehiclePawn.h"

void AFuelPickup::OnPickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitInfo)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap event occurred in FuelPickup"))
	APCGVehiclePawn* VehiclePawn = Cast<APCGVehiclePawn>(OtherActor);
	if(VehiclePawn)
	{
		// Adds fuel to the vehicle then removes pickup.
		UFuelComponent* FuelComponent = VehiclePawn->GetComponentByClass<UFuelComponent>();
		if(FuelComponent)
		{
			FuelComponent->AddFuel(FuelAmount);
		}
		Destroy();
	}
	

}
