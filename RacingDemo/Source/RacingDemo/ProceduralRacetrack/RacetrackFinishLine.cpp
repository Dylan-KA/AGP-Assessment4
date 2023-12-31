// Fill out your copyright notice in the Description page of Project Settings.


#include "RacetrackFinishLine.h"

#include "RacingDemo/GameManagers/MyRacingGameMode.h"
#include "RacingDemo/PlayerVehicle/PCGVehiclePawn.h"

// Sets default values
ARacetrackFinishLine::ARacetrackFinishLine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FinishCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Finish Collider"));
	SetRootComponent(FinishCollider);
	
	bReplicates = true;
}

// Called when the game starts or when spawned
void ARacetrackFinishLine::BeginPlay()
{
	Super::BeginPlay();

	// The server should be responsible for determining if a vehicle has reached the finish line
	// and what to do once this occurs
	if (GetNetMode() == NM_Client)
	{
		return;
	}
	
	if (FinishCollider)
	{
		// This attaches OnFinishOverlap function to be called when the OnComponentBeginOverlap event is triggered.
		FinishCollider->OnComponentBeginOverlap.AddDynamic(this, &ARacetrackFinishLine::OnFinishOverlap);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FinishCollider is null in the RacetrackFinishLine class."))
	}
	
}

void ARacetrackFinishLine::OnFinishOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitInfo)
{
	if (APCGVehiclePawn* VehiclePawn = Cast<APCGVehiclePawn>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("A Vehicle has reached the finish line"))
		VehiclePawn->bHasWonRace = true;
		// Start the restart timer
		AMyRacingGameMode* GameMode = Cast<AMyRacingGameMode>(GetWorld()->GetAuthGameMode());
		GameMode->StartRestartTimer();

	}
}

// Called every frame
void ARacetrackFinishLine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

