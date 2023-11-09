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

	// Called after all players have joined the game
	virtual void HandleMatchHasStarted() override;
	
	// Iterate through all vehicle pawns and call start restart timer
	void StartRestartTimer();

	void RestartRace();

protected:
	
	// Moves the given player to the appropriate start position based on the number of players
	void MovePlayerToStart(APlayerController* NewPlayer);

	// An actor that has not yet been moved to start
	// Will be moved once track is finished
	UPROPERTY()
	APlayerController* ServerPlayerController;
	
	UPROPERTY()
	AProceduralRacetrackActor* ProceduralRacetrackActor;

	// Keep track of players, used to determine where to spawn players.
	UPROPERTY()
	int32 PlayerIndex = 0;
	
};
