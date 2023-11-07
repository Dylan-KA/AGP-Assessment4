// Fill out your copyright notice in the Description page of Project Settings.


#include "RacetrackFinishLine.h"
#include "RacingDemo/PlayerVehicle/PCGVehiclePawn.h"

// Sets default values
ARacetrackFinishLine::ARacetrackFinishLine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FinishCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Finish Collider"));
	FinishCollider->SetupAttachment(GetRootComponent());

	// Should only be on the server
	bReplicates = false;
	// The server should be responsible for determining if a vehicle has reached the finish line
	// and what to do once this occurs
	
}

// Called when the game starts or when spawned
void ARacetrackFinishLine::BeginPlay()
{
	Super::BeginPlay();

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
	}
}

// Called every frame
void ARacetrackFinishLine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

