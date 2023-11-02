// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Actor.h"
//#include "GameFramework/PlayerStart.h"
#include "RacingDemo/Pathfinding/PathfindingSubsystem.h"
#include "RoadSplineMeshActor.h"
#include "ProceduralRacetrackActor.generated.h"

class UProceduralMeshComponent;
UCLASS()
class RACINGDEMO_API AProceduralRacetrackActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduralRacetrackActor();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FVector GetStartPosition();

	FVector GetEndPosition();
	

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

	//UPROPERTY()
	//APlayerStart* PlayerStart; 

	// Meshes 
	UPROPERTY()
	TArray<ARoadSplineMeshActor*> RoadMeshActors;
	UPROPERTY()
	TArray<AStaticMeshActor*> TreeMeshesActors;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UStaticMesh*> TreeMeshes;
	
	UPROPERTY(EditAnywhere)
	bool bShouldRegenerate = true;
	
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
	void FindTrackPath();
	// spawns in road meshes
	void SpawnTrack();
	void ClearTrack();
	
	// spawns in trees 
	void GenerateTreeSpawnPositions();
	void SpawnTrees();
	
	FVector GetPointOnEdge(int32 EdgeIndex);

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

	// Track is generated on server and replicated to clients
	UPROPERTY(Replicated)
	TArray<FVector> Track;
	
	// Replicate this variable to all clients
	//TArray<FVector> TreeSpawnPositions;

	/*void GenerateTrackImplementation(FVector Start, FVector End, FVector Point1, FVector Point2);
	UFUNCTION(Server, Reliable)
	void ServerGenerateTrack(FVector Start, FVector End, FVector Point1, FVector Point2);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastGenerateTrack(FVector Start, FVector End, FVector Point1, FVector Point2);*/

};
