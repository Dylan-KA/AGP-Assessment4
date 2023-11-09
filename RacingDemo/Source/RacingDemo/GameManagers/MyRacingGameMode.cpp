// Fill out your copyright notice in the Description page of Project Settings.


#include "MyRacingGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "RacingDemo/PlayerVehicle/PCGVehiclePawn.h"

void AMyRacingGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Getting the ProceduralRacetrackActor
	ProceduralRacetrackActor = Cast<AProceduralRacetrackActor>(
		UGameplayStatics::GetActorOfClass(GetWorld(), AProceduralRacetrackActor::StaticClass()));
}

// Called after a successful login
void AMyRacingGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// Getting the ProceduralRacetrackActor
	ProceduralRacetrackActor = Cast<AProceduralRacetrackActor>(
		UGameplayStatics::GetActorOfClass(GetWorld(), AProceduralRacetrackActor::StaticClass()));
	
	//UE_LOG(LogTemp, Warning, TEXT("PostLogin executed from MyRacingGameMode"))
	
	if (NewPlayer)
	{
		MovePlayerToStart(NewPlayer);
	} else
	{
		//UE_LOG(LogTemp, Error, TEXT("APlayerController* NewPlayer from PostLogin is NULL"))
	}
}

// Called after all players have joined
void AMyRacingGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	//UE_LOG(LogTemp, Warning, TEXT("MatchHasStarted"))

	// Since the ListenServer's vehicle spawns before the track has generated,
	// it is moved to the start at this point rather than when it joins in PostLogin
	if (ServerPlayerController != nullptr)
	{
		MovePlayerToStart(ServerPlayerController);
	} else
	{
		//UE_LOG(LogTemp, Error, TEXT("ServerPlayerController is NULL"))
	}
}

// Iterate through all vehicle pawns and call start restart timer
void AMyRacingGameMode::StartRestartTimer()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PlayerController = It->Get();

		if (PlayerController)
		{
			APCGVehiclePawn* VehiclePawn = Cast<APCGVehiclePawn>(PlayerController->GetPawn());
			VehiclePawn->StartRestartTimer();
		}
	}
}

void AMyRacingGameMode::RestartRace()
{
	GetWorld()->ServerTravel("VehicleExampleMap");
}

// Moves the given player to the start of the track
// and positions to the left or right depending on the number of players.
// Called when either the PlayerController joins the game or when the match has started.
void AMyRacingGameMode::MovePlayerToStart(APlayerController* NewPlayer)
{
	APawn* PlayerPawn = NewPlayer->GetPawn();
	if (PlayerPawn == nullptr)
	{
		//UE_LOG(LogTemp, Error, TEXT("PlayerPawn of NewPlayer is NULL"))
	}
	
	if (ProceduralRacetrackActor == nullptr)
	{
		//UE_LOG(LogTemp, Error, TEXT("ProceduralRacetrackActor is NULL"))
		return;
	}

	if (ProceduralRacetrackActor->HasGenerated())
	{
		//UE_LOG(LogTemp, Warning, TEXT("HAS GENERATED: MOVING PLAYER TO START"))

		// Calculate start positions
		FVector ForwardVector = ProceduralRacetrackActor->GetTrackStart().ForwardRotation.Vector();
		FVector OffsetVector = 400.0f * FVector(-ForwardVector.Y, ForwardVector.X, ForwardVector.Z);

		FVector StartPosition1 = ProceduralRacetrackActor->GetTrackStart().Position - OffsetVector;
		FVector StartPosition2 = ProceduralRacetrackActor->GetTrackStart().Position + OffsetVector;

		if (PlayerIndex == 0) // Spawn first player to the left
		{
			UE_LOG(LogTemp, Warning, TEXT("Spawning Player to the left"))
			PlayerPawn->SetActorLocationAndRotation(StartPosition1,
				ProceduralRacetrackActor->GetTrackStart().ForwardRotation,
				false, nullptr, ETeleportType::TeleportPhysics);
		} else if (PlayerIndex == 1) // Spawn second player to the right
		{
			UE_LOG(LogTemp, Warning, TEXT("Spawning Player to the right"))
			PlayerPawn->SetActorLocationAndRotation(StartPosition2,
				ProceduralRacetrackActor->GetTrackStart().ForwardRotation,
				false, nullptr, ETeleportType::TeleportPhysics);
		}
		PlayerIndex += 1;
	} else
	{
		//UE_LOG(LogTemp, Error, TEXT("NOT YET GENERATED"))
		ServerPlayerController = NewPlayer;
		if (ServerPlayerController)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Set ServerPlayerPawn"))
		}
	}
}
