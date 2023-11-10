// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Actor.h"
#include "RacingDemo/Pathfinding/PathfindingSubsystem.h"
#include "RoadSplineMeshActor.h"
#include "RacingDemo/Pickups/FuelPickup.h"
#include "RacetrackFinishLine.h"
#include "RampActor.h"
#include "Engine/DirectionalLight.h"
#include "RacingDemo/GameManagers/RacingGameInstance.h"
#include "ProceduralRacetrackActor.generated.h"

USTRUCT()
struct FTree
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FVector Position;
	UPROPERTY()
	FRotator Rotation;
	UPROPERTY()
	int32 MeshIndex;
	UPROPERTY()
	float Scale;
};

USTRUCT()
struct FTrackSection
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FVector Position;
	UPROPERTY()
	FRotator ForwardRotation;
	UPROPERTY()
	FVector Midpoint; 
	bool operator==(const FTrackSection& Other)
	{
		return (Position == Other.Position && ForwardRotation == Other.ForwardRotation); 
	}
};


class UProceduralMeshComponent;
UCLASS()
class RACINGDEMO_API AProceduralRacetrackActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduralRacetrackActor();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FTrackSection GetTrackStart() const;

	FTrackSection GetTrackEnd() const;

	bool HasGenerated() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* ProceduralMesh;

	// Randomise Sun Rotation
	//UFUNCTION(BlueprintImplementableEvent)
	//void RandomiseSunRotation();

	void GenerateRacetrackLevel();
	
	// Pathfinding Subsystem
	UPROPERTY()
	UPathfindingSubsystem* PathfindingSubsystem;
	UPROPERTY()
	const URacingGameInstance* GameInstance;
	UPROPERTY()
	TArray<FVector> Waypoints;
	
	// Map Generation Settings.
	UPROPERTY(EditAnywhere)
	int32 Width = 10;
	UPROPERTY(EditAnywhere)
	int32 Height = 10;
	UPROPERTY(EditAnywhere)
	float VertexSpacing = 1000.0f;
	UPROPERTY(EditAnywhere)
	int32 TreeAmount = 50;

	UPROPERTY()
	TArray<FVector> Vertices;
	
	UPROPERTY()
	AStaticMeshActor* StartFlagMeshActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* StartFlagMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ADirectionalLight* Sun;
	
	// Meshes 
	UPROPERTY()
	TArray<ARoadSplineMeshActor*> RoadMeshActors;
	UPROPERTY()
	TArray<AStaticMeshActor*> TreeMeshActors;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UStaticMesh*> TreeMeshes;
	UPROPERTY()
	TArray<AFuelPickup*> FuelPickups;
	UPROPERTY()
	TArray<ARampActor*> Ramps; 
	
	UPROPERTY(EditAnywhere)
	bool bHasGenerated = false;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// Loop through width and height to get the appropriate grid positions
	void GenerateGrid();
	// Find the top, bottom, left and right edge indexes and store them in the appropriate array
	void InitialiseIndexes();
	void ClearIndexes();
	void RandomiseStartAndEnd();
	void RandomiseCheckpoint();
	
	// Pathfinding to get track 
	void GenerateTrack();
	void GenerateTrees();

	// Spawns in road
	UFUNCTION()
	void SpawnTrack();
	void ClearTrackMeshes();
	void ClearTrackValues();
	// spawns in trees
	UFUNCTION()
	void SpawnTrees();

	// Spawn fuel pickup items randomly across the track 
	void SpawnFuelPickups();
	void SpawnRamps();

	void SpawnStartLine();
	// Spawns in a collision box at the finish line
	void SpawnFinishLine();
	
	// Helper Functions 
	FVector GetPointOnEdge(int32 EdgeIndex);
	void PrintTrack();
	FVector GetRelativePosition(FVector Position, FRotator ForwardRotation, FString Direction); 

	// Indexes of Grid
	TArray<int32> BottomEdgeIndexes;
	TArray<int32> TopEdgeIndexes;
	TArray<int32> LeftEdgeIndexes;
	TArray<int32> RightEdgeIndexes;
	TArray<int32> CentralIndexes;

	// Track positions
	FVector StartPosition;
	FVector Checkpoint1;
	FVector Checkpoint2;
	FVector EndPosition;
	
	TArray<FTrackSection> PossibleTrackSpawnPositions;

	// Replicated properties
	// Track is generated on server and replicated to clients
	UPROPERTY(ReplicatedUsing=SpawnTrack)
	TArray<FTrackSection> Track;
	// Tree values include position, rotation and mesh type
	UPROPERTY(ReplicatedUsing=SpawnTrees)
	TArray<FTree> TreeValues;
//	UPROPERTY(ReplicatedUsing=RotateSun)
//	FRotator SunRotation; 

};
