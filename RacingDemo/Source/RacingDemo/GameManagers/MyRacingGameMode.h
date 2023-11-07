// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "RacingDemo/ProceduralRacetrack/ProceduralRacetrackActor.h"
#include "MyRacingGameMode.generated.h"

/**
 * 
 */
UCLASS()
class RACINGDEMO_API AMyRacingGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:

	virtual void BeginPlay() override;
	
	// Called after a successful login.
	// This is the first place it is safe to call replicated functions on the PlayerController.
	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void HandleMatchHasStarted() override;
	
	// Your MovePlayersToStart function.
	void MovePlayerToStart(APlayerController* NewPlayer);

	// An actor that has not yet been moved to start
	// The RaceTrackActor will move it once finished
	UPROPERTY()
	APlayerController* ServerPlayerController;
	
protected:

	// Gets all vehicle pawns and based on the number of players,
	// positions them appropriately at the start of the track
	UPROPERTY()
	AProceduralRacetrackActor* ProceduralRacetrackActor;

	// Keep track of players, used to determine where to spawn players.
	UPROPERTY()
	int32 PlayerIndex = 0;
	
};
