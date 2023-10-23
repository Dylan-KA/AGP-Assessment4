// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerStart.h"
#include "RacingDemo/Pathfinding/PathfindingSubsystem.h"
#include "RacingDemo/RoadSplineMeshActor.h"
#include "ProceduralRacetrackActor.generated.h"

class UProceduralMeshComponent;
UCLASS()
class RACINGDEMO_API AProceduralRacetrackActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduralRacetrackActor();

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
	bool bShouldRegenerate;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void GenerateGrid();
	void InitialiseIndexes();
	void ClearIndexes();
	void RandomiseStartAndEnd();
	void RandomiseCheckpoint();
	void FindTrackPath();
	void BuildTrack();
	void ClearTrack();
	
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
	TArray<FVector> Track;
};
