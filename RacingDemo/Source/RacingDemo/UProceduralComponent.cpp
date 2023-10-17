// Fill out your copyright notice in the Description page of Project Settings.

#include "UProceduralComponent.h"
#include "PCGVehiclePawn.h"

// Sets default values for this component's properties
UProceduralComponent::UProceduralComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}

// Called when the game starts
void UProceduralComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Generates random vehicle stats
void UProceduralComponent::GenerateRandomVehicle(EVehicleRarity& VehicleRarity, FVehicleStats& VehicleStats)
{
	VehicleRarity = VehicleRarityPicker();
	TArray<bool> GoodStats;
	switch (VehicleRarity)
	{
	case EVehicleRarity::Legendary:
		GoodStats = VehicleStatPicker(6, 6);
		UE_LOG(LogTemp, Warning, TEXT("Rarity: Legendary"))
		break;
	case EVehicleRarity::Master:
		GoodStats = VehicleStatPicker(5, 6);
		UE_LOG(LogTemp, Warning, TEXT("Rarity: Master"))
		break;
	case EVehicleRarity::Rare:
		GoodStats = VehicleStatPicker(4, 6);
		UE_LOG(LogTemp, Warning, TEXT("Rarity: Rare"))
		break;
	case EVehicleRarity::Common:
		GoodStats = VehicleStatPicker(2, 6);
		UE_LOG(LogTemp, Warning, TEXT("Rarity: Common"))
		break;
	default:
		GoodStats = VehicleStatPicker(2, 6);
		UE_LOG(LogTemp, Warning, TEXT("Rarity: Default"))
		break;
	}

	// Randomly choose the stats of the vehicle depending on whether each is a good or bad stat
	VehicleStats.Acceleration = GoodStats[0] ? FMath::RandRange(0.8f, 1.0f) : FMath::RandRange(0.3f, 0.79f);
	VehicleStats.ReverseAcceleration = GoodStats[1] ? FMath::RandRange(0.8f, 1.0f) : FMath::RandRange(0.3f, 0.79f);
	VehicleStats.TopSpeed = GoodStats[2] ? FMath::RandRange(2000, 5000) : FMath::RandRange(5000, 8000);
	VehicleStats.WeightDistribution = GoodStats[3] ? FMath::RandRange(0.0f, 50.0f) : FMath::RandRange(-50.0f, -150.0f);
	VehicleStats.MaxFuelCapacity = GoodStats[4] ? FMath::RandRange(10.0f, 20.0f) : FMath::RandRange(1.0f, 5.0f);
	VehicleStats.TurningSpeed = GoodStats[5] ? FMath::RandRange(0.8f, 0.99f) : FMath::RandRange(0.5f, 0.79f);
	
	UE_LOG(LogTemp, Warning, TEXT("%s"), *VehicleStats.ToString())
	
}

// Chooses a random Vehicle rarity with weighted chances
EVehicleRarity UProceduralComponent::VehicleRarityPicker()
{
	// Rules:
	// 50% chance of Common
	// 30% chance of Rare
	// 15% chance of Master
	// 5% chance of Legendary
	const float RandPercent = FMath::RandRange(0.0f, 1.0f);
	
	if (RandPercent <= 0.5f)
	{
		return EVehicleRarity::Common;
	}
	if (RandPercent <= 0.8f)
	{
		return EVehicleRarity::Rare;
	}
	if (RandPercent <= 0.95f)
	{
		return EVehicleRarity::Master;
	}
	
	return EVehicleRarity::Legendary;
}

// Chooses which Stats are good and which are bad
TArray<bool> UProceduralComponent::VehicleStatPicker(int32 NumOfGood, int32 NumOfStats)
{
	// Fill the array with the correct number of good and bad stats.
	TArray<bool> GoodStats;
	for (int32 i = 0; i < NumOfStats; i++)
	{
		// Ternary condition: Will add true if I < NumOfGood otherwise add false.
		GoodStats.Add(i < NumOfGood ? true : false);
	}

	// Array shuffling algorithm.
	for (int32 i = 0; i < GoodStats.Num(); i++)
	{
		// Get a random index from the GoodStats array.
		const int32 RandIndex = FMath::RandRange(0, GoodStats.Num() - 1);
		// Then swap the item at that random index with the item in the i index.
		const bool Temp = GoodStats[i];
		GoodStats[i] = GoodStats[RandIndex];
		GoodStats[RandIndex] = Temp;
	}

	return GoodStats;
}

// Called every frame
void UProceduralComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}


