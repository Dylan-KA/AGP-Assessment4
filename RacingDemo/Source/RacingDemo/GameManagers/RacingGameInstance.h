// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "WheeledVehiclePawn.h"
#include "RacingDemo/Pickups/FuelPickup.h"
#include "RacingDemo/ProceduralRacetrack/RampActor.h"
#include "RacingDemo/ProceduralRacetrack/RoadSplineMeshActor.h"
#include "RacingGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class RACINGDEMO_API URacingGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UClass* GetVehicleClass() const;

	UClass* GetRoadMeshClass() const;

	UClass* GetVehiclePlayerController() const;

	UClass* GetFuelPickupClass() const; 

	UClass* GetRampClass() const; 

protected:

	UPROPERTY(EditDefaultsOnly, Category="Vehicle Class")
	TSubclassOf<AWheeledVehiclePawn> VehicleClass;

	UPROPERTY(EditDefaultsOnly, Category="Road Mesh Classes")
	TSubclassOf<ARoadSplineMeshActor> RoadSplineMeshClass;

	UPROPERTY(EditDefaultsOnly, Category="Vehicle Player Controller Class")
	TSubclassOf<APlayerController> VehiclePlayerController;
	
	UPROPERTY(EditDefaultsOnly, Category="Fuel Pickup Classes")
	TSubclassOf<AFuelPickup> FuelPickupClass;

	UPROPERTY(EditDefaultsOnly, Category="Ramp Classes")
	TSubclassOf<ARampActor> RampClass;
	
};
