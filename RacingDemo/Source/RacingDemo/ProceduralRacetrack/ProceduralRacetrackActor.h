// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Actor.h"
#include "RacingDemo/Pathfinding/PathfindingSubsystem.h"
#include "RoadSplineMeshActor.h"
#include "RacetrackFinishLine.h"
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

	void GenerateRacetrackLevel();
	
	// Pathfinding Subsystem
	UPROPERTY()
	UPathfindingSubsystem* PathfindingSubsystem;
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

	// Meshes 
	UPROPERTY()
	TArray<ARoadSplineMeshActor*> RoadMeshActors;
	UPROPERTY()
	TArray<AStaticMeshActor*> TreeMeshActors;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UStaticMesh*> TreeMeshes;
	
	UPROPERTY(EditAnywhere)
	bool bHasGenerated = false;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	float Time;

	
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

	// Spawns in a collision box at the finish line
	void SpawnFinishLine();
	
	// Helper Function 
	FVector GetPointOnEdge(int32 EdgeIndex);
	void PrintTrack();

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

	// Replicated properties
	// Track is generated on server and replicated to clients
	UPROPERTY(ReplicatedUsing=SpawnTrack)
	TArray<FTrackSection> Track;
	// Tree values include position, rotation and mesh type
	UPROPERTY(ReplicatedUsing=SpawnTrees)
	TArray<FTree> TreeValues;
	

};
