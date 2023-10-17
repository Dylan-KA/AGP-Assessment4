// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "WheeledVehiclePawn.h"
#include "RacingGameInstance.generated.h"

class AWeaponPickup;
/**
 * 
 */
UCLASS()
class RACINGDEMO_API URacingGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UClass* GetVehicleClass() const;

protected:

	UPROPERTY(EditDefaultsOnly, Category="Vehicle Class")
	TSubclassOf<AWheeledVehiclePawn> VehicleClass;
	
};
