// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawningSubsystem.h"
#include "FuelPickup.h"
#include "RacingDemo/GameManagers/RacingGameInstance.h"

/*void USpawningSubsystem::SpawnPickups(TArray<FTrackSection> Track)
{
	// add all track positions to the possible spawns list
	for (auto TrackSection : Track)
	{
		PossibleSpawnPositions.Add(TrackSection.Position);
	}
	// Number of fuel pickups is  dependent on length of the track
	for(int i = 0; i < Track.Num()/10; i++)
	{
		//Randomly select position
		FVector SpawnPosition = PossibleSpawnPositions[FMath::RandRange(0,PossibleSpawnPositions.Num()-1)];
		if (const URacingGameInstance* GameInstance =
				GetWorld()->GetGameInstance<URacingGameInstance>())
		{
			//Spawn fuel pickup at position
			AFuelPickup* FuelPickup = GetWorld()->SpawnActor<AFuelPickup>(
			GameInstance->GetFuelPickupClass(), SpawnPosition, FRotator(0,0,0));
		}
		//Remove position from list
		PossibleSpawnPositions.Remove(SpawnPosition);
	}
	
}*/
