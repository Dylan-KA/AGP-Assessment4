// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupBase.h"
#include "FuelPickup.generated.h"

/**
 * 
 */
UCLASS()
class RACINGDEMO_API AFuelPickup : public APickupBase
{
	GENERATED_BODY()
	
protected:

	// Adds fuel to the vehicle then removes pickup.
	virtual void OnPickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitInfo) override;

	float FuelAmount = 3.0f;
	
};
