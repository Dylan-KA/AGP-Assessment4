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

// Struct of tree actor values used for replication  
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
// struct of track section values
// Used when spawning items along the track
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

	// Get the first section of the track
	FTrackSection GetTrackStart() const;

	// Get the penultimate section of the track
	FTrackSection GetTrackEnd() const;

	// Returns true if the track has been generated 
	bool HasGenerated() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Mesh Compenent to give this actor a transform 
	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* ProceduralMesh;

	// Generate the grid, racetrack details, track, trees
	// pickup items and other meshee
	void GenerateRacetrackLevel();
	
	// Pathfinding Subsystem
	UPROPERTY()
	UPathfindingSubsystem* PathfindingSubsystem;
	// Game Instance 
	UPROPERTY()
	const URacingGameInstance* GameInstance;
	
	// Map Generation Settings.
	UPROPERTY(EditAnywhere)
	int32 Width = 10;
	UPROPERTY(EditAnywhere)
	int32 Height = 10;
	UPROPERTY(EditAnywhere)
	float VertexSpacing = 1000.0f;
	UPROPERTY(EditAnywhere)
	int32 TreeAmount = 50;
	// list of all positions on the grid
	UPROPERTY()
	TArray<FVector> Vertices;
	
	// Reference to the direction light in the level
	// Blueprints are used to set it's rotation randomly
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ADirectionalLight* Sun;
	
	// Meshes //
	
	// Static mesh for the start flag 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* StartFlagMesh;
	UPROPERTY()
	AStaticMeshActor* StartFlagMeshActor;
	// List of all road spline mesh actors
	UPROPERTY()
	TArray<ARoadSplineMeshActor*> RoadMeshActors;
	// List of all tree static meshes
	UPROPERTY()
	TArray<AStaticMeshActor*> TreeMeshActors;
	// Set list of tree meshes, that can be changed in the editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UStaticMesh*> TreeMeshes;
	// List of all fuel pickup actors
	UPROPERTY()
	TArray<AFuelPickup*> FuelPickups;
	// List of all ramp actors
	UPROPERTY()
	TArray<ARampActor*> Ramps; 

	// Boolean to indicate whether the track has generated
	UPROPERTY(EditAnywhere)
	bool bHasGenerated = false;

private:
	//// Indexes of Grid ////

	// External Indexes
	TArray<int32> BottomEdgeIndexes;
	TArray<int32> TopEdgeIndexes;
	TArray<int32> LeftEdgeIndexes;
	TArray<int32> RightEdgeIndexes;
	// Internal Indexes
	TArray<int32> CentralIndexes;

	//// Key Track Positions ////
	FVector StartPosition;
	FVector Checkpoint1;
	FVector Checkpoint2;
	FVector EndPosition;

	// List of available spawn positions on the track
	// Used when spawning pickups and ramps
	TArray<FTrackSection> PossibleTrackSpawnPositions;
	
	//// Replicated Properties ////
	// List of Track Sections
	// Generated on server and replicated to clients
	UPROPERTY(ReplicatedUsing=SpawnTrack)
	TArray<FTrackSection> Track;
	// Tree values include position, rotation and mesh type
	UPROPERTY(ReplicatedUsing=SpawnTrees)
	TArray<FTree> TreeValues;
	
	//// Generative Functions //// 
	
	// Loop through Width and Height, using the VertexSpacing to determine the Vertices of the grid
	void GenerateGrid();
	// Calculate the top, bottom, left and right edge indexes and store them in lists
	void InitialiseIndexes();
	// Randomly select two different edges of the grid, and two points from those edges
	// to use as the start and end positions
	void RandomiseStartAndEnd();
	// Randomly select two different positions from the middle of the grid
	void RandomiseCheckpoint();
	// Path find between all checkpoints to populate the track
	void GenerateTrack();
	// Randomly generate positions, rotations, scales and meshes for each tree
	// the number of trees is determined by the TreeAmount property
	void GenerateTrees();

	//// Spawning Functions ////
	
	// Spawns in road splines for all track sections
	UFUNCTION()
	void SpawnTrack();
	// Spawns in all trees using the list of Tree Values
	UFUNCTION()
	void SpawnTrees();
	
	// Randomly spawns in rows of 3 fuel pickup items along the track 
	void SpawnFuelPickups();
	// Randomly spawns single ramps along the track
	void SpawnRamps();
	// Spawns in a mesh actor for the start flag at the beginning of the track
	void SpawnStartLine();
	// Spawns in a RacetrackFinishLine Actor at the end of the track
	void SpawnFinishLine();

	
	//// Clearing Functions ////
	// Destroy all meshes and spawned actors
	void ClearMeshes();
	// Empty all lists storing track values
	// including the track array itself
	void ClearTrackValues();
	// Empty all index lists
	void ClearIndexes();

	
	//// Helper Functions ////
	
	/**
	* Will retrieve a path from the StartLocation, to a random position in the world's navigation system.
	* @param EdgeIndex a value of 1 to 4 that corresponds to an edge of a quadrilateral 
	* @return An FVector Position along the given edge 
	*/
	FVector GetPointOnEdge(int32 EdgeIndex);
	// Print each track position and rotation in order
	void PrintTrack();
	/**
	* Will retrieve a path from the StartLocation, to a random position in the world's navigation system.
	* @param Position The location that the resulting position will be adjacent to
	* @param ForwardRotation The forward orientation
	* @param Direction The desired direction of the adjacent position in string format ("Left" or "Right")
	* @return An FVector Position that is adjacent to the given values in the given direction 
	 */
	FVector GetAdjacentPosition(FVector Position, FRotator ForwardRotation, FString Direction); 

};
