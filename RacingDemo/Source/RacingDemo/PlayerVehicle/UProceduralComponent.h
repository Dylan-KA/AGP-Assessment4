// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UProceduralComponent.generated.h"

struct FVehicleStats;

UENUM(BlueprintType)
enum class EVehicleRarity : uint8
{
	Common = 0,
	Rare = 1,
	Master = 2,
	Legendary = 3
};

/**
 * This Component is responsible for generating the rarity and stats of the vehicle and populating the corresponding member variables in the VehicleStatsManager
 */
UCLASS( BlueprintType, Blueprintable )
class RACINGDEMO_API UProceduralComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProceduralComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Chooses a random Vehicle rarity with weighted chances
	EVehicleRarity VehicleRarityPicker();
	
	// Chooses which Stats are good and which are bad
	TArray<bool> VehicleStatPicker(int32 NumOfGood, int32 NumOfStats);
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Generates random vehicle stats
	void GenerateRandomVehicle(EVehicleRarity& VehicleRarity, FVehicleStats& VehicleStats);
		
};
