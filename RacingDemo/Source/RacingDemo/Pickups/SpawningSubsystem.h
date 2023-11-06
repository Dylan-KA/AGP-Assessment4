// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RacingDemo/ProceduralRacetrack/ProceduralRacetrackActor.h"
#include "Subsystems/WorldSubsystem.h"
#include "SpawningSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class RACINGDEMO_API USpawningSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void SpawnPickups(TArray<FTrackSection> Track);
};
