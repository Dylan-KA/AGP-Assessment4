// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralRacetrackActor.h"
#include "ProceduralMeshComponent.h"
#include "RacingDemo/GameManagers/RacingGameInstance.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AProceduralRacetrackActor::AProceduralRacetrackActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Procedural Mesh"));
	SetRootComponent(ProceduralMesh);
}

void AProceduralRacetrackActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AProceduralRacetrackActor, Track); 
}

FVector AProceduralRacetrackActor::GetStartPosition()
{
	return StartPosition;
}

FVector AProceduralRacetrackActor::GetEndPosition()
{
	return EndPosition;
}

// Called when the game starts or when spawned
void AProceduralRacetrackActor::BeginPlay()
{
	Super::BeginPlay();
	//UE_LOG(LogTemp, Warning, TEXT("BeginPlay Starting"));

	PathfindingSubsystem = GetWorld()->GetSubsystem<UPathfindingSubsystem>();
	//UE_LOG(LogTemp, Warning, TEXT("Begin Play Finished: %d"), GetNetMode()); 

}

void AProceduralRacetrackActor::GenerateRacetrackLevel()
{
	// if on the server, generate the track and grid
	if(GetNetMode() < ENetMode::NM_Client)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Generating Track - Server"))
		ClearTrack();
		GenerateGrid();
		InitialiseIndexes();
		RandomiseStartAndEnd();
		RandomiseCheckpoint();
		FindTrackPath();
	}
	// spawn the track on both server and client
	SpawnTrack(); 


}

// Called every frame
void AProceduralRacetrackActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Time += DeltaTime;
	//UE_LOG(LogTemp, Error, TEXT("Time: %f"), Time)
	// wait for both server and client begin play to finish before generating level
	if(Time > 1 && bShouldRegenerate)
	{
		if(PathfindingSubsystem)
		{
			bShouldRegenerate = false; 
			GenerateRacetrackLevel(); 
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Can't find the pathfinding subsystem"))
		}
	}
}

void AProceduralRacetrackActor::GenerateGrid()
{
	for (int32 Y =  0; Y < Height; Y++)
	{
		for (int32 X = 0; X < Width; X++)
		{
			FVector VertexLocation = FVector(X * VertexSpacing, Y * VertexSpacing, 0.0f);
			VertexLocation.Z += 0.1;
			Vertices.Add(VertexLocation);
			
			// DrawDebugSphere(GetWorld(), VertexLocation, 50.0f, 8, FColor::Blue,
			// 	true, -1, 0, 10.0f);
		}
	}
	if(PathfindingSubsystem)
	{
		PathfindingSubsystem->PlaceProceduralNodes(Vertices, Width, Height);
		Waypoints = PathfindingSubsystem->GetNodePositions();
	}
}

void AProceduralRacetrackActor::InitialiseIndexes()
{
	// Clear Indexes
	ClearIndexes();
	// fill lists with the indexes of each node on the edge of the grid
	UE_LOG(LogTemp, Warning, TEXT("Height : %d"), Height);
	UE_LOG(LogTemp, Warning, TEXT("Width : %d"), Width);
	
	// ignore the left and right rows
	for(int32 i = 1; i < Width - 1; i++)
	{
		TopEdgeIndexes.Add((Width * Height - 1) - i);
		BottomEdgeIndexes.Add(i);
	}
	
	int32 RightIndex = Width;
	int32 LeftIndex = Width + (Width - 1);
	// ignore the top and bottom rows
	for(int32 i = 1; i < Height - 1; i++)
	{
		RightEdgeIndexes.Add(RightIndex);
		LeftEdgeIndexes.Add(LeftIndex);

		RightIndex += Width;
		LeftIndex += Width;
	}
	
	// find all indexes which aren't on the edges
	for(int32 i = Width; i < (Width * Height) - (Width * 2); i++)
	{
		// check if index is on the left or right edge
		if(LeftEdgeIndexes.Contains(i) || RightEdgeIndexes.Contains(i))
		{
			continue;
		}
		CentralIndexes.Add(i);
	}

	UE_LOG(LogTemp, Warning, TEXT("Left Edge Length: %d"), LeftEdgeIndexes.Num());
	UE_LOG(LogTemp, Warning, TEXT("Right Edge Length: %d"), RightEdgeIndexes.Num());
	UE_LOG(LogTemp, Warning, TEXT("Top Edge Length: %d"), RightEdgeIndexes.Num());
	UE_LOG(LogTemp, Warning, TEXT("Bottom Edge Length: %d"), RightEdgeIndexes.Num());
}

void AProceduralRacetrackActor::ClearIndexes()
{
	LeftEdgeIndexes.Empty();
	RightEdgeIndexes.Empty();
	TopEdgeIndexes.Empty();
	BottomEdgeIndexes.Empty();
	CentralIndexes.Empty();
}

void AProceduralRacetrackActor::RandomiseStartAndEnd()
{
	TArray<int32> Edges;
	Edges.Add(1);
	Edges.Add(2);
	Edges.Add(3);
	Edges.Add(4);

	// randomly select two different edges
	int32 StartEdge = Edges[FMath::RandRange(0,Edges.Num() - 1)];
	Edges.Remove(StartEdge);
	int32 EndEdge = Edges[FMath::RandRange(0, Edges.Num() - 1)];

	StartPosition = GetPointOnEdge(StartEdge);
	EndPosition = GetPointOnEdge(EndEdge);
}

void AProceduralRacetrackActor::RandomiseCheckpoint()
{
	// could improve this to allow the number of checkpoints to be changed in the editor
	// get random central checkpoint
	int32 CheckpointIndex1 = CentralIndexes[FMath::RandRange(0, CentralIndexes.Num() - 1)];
	Checkpoint1 = Waypoints[CheckpointIndex1];
	CentralIndexes.Remove(CheckpointIndex1);
	
	int32 CheckpointIndex2 = CentralIndexes[FMath::RandRange(0, CentralIndexes.Num() - 1)];
	Checkpoint2 = Waypoints[CheckpointIndex2];
}

void AProceduralRacetrackActor::FindTrackPath()
{
	UE_LOG(LogTemp, Warning, TEXT("Start: %s"), *StartPosition.ToString());
	UE_LOG(LogTemp, Warning, TEXT("End: %s"), *EndPosition.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Netmode: %d"), GetNetMode());
	
	// Fix track generation so the path doesn't backtrack over itself
	// Find path from start point to checkpoints to end point
	Track = PathfindingSubsystem->GetPath(StartPosition, Checkpoint1);
	Track.Pop(); 
	Track.Append(PathfindingSubsystem->GetPath(Checkpoint1, Checkpoint2));
	Track.Pop(); 
	Track.Append(PathfindingSubsystem->GetPath(Checkpoint2, EndPosition));
}

void AProceduralRacetrackActor::SpawnTrack()
{
	UE_LOG(LogTemp, Warning, TEXT("Spawn Track Implementation"));
	UE_LOG(LogTemp, Warning, TEXT("Netmode: %d"), GetNetMode());
	UE_LOG(LogTemp, Warning, TEXT("Track Length: %d"), Track.Num());



	FVector PrevPosition;
	// for every point on the track spawn in a road mesh 
	for (int32 i = 0; i < Track.Num(); i++)
	{
		FVector CurrentPosition = Track[i];
		// if the previous position has been set 
		if( i != 0 )
		{
			FVector MidPoint = (CurrentPosition + PrevPosition) / 2;
			FVector Diff = CurrentPosition - PrevPosition;
			PrevPosition.Z += 1;
			//UE_LOG(LogTemp, Warning, TEXT("Step: %s"), *PrevPosition.ToString());
			// Spawn a road at each point and face it towards the next point on the track
			FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(PrevPosition, CurrentPosition);
			if (const URacingGameInstance* GameInstance =
				GetWorld()->GetGameInstance<URacingGameInstance>())
			{

				ARoadSplineMeshActor* RoadMeshActor = GetWorld()->SpawnActor<ARoadSplineMeshActor>(
				GameInstance->GetRoadMeshClass(), PrevPosition,Rotation);
				RoadMeshActors.Add(RoadMeshActor);
				RoadMeshActor->GetSplineMeshComponent()->SetStartPosition(FVector(-200 ,0.0, 1.0 ));
				RoadMeshActor->GetSplineMeshComponent()->SetEndPosition(FVector(Diff.Length(),0.0, 1.0 ));
				
			}
			
			// if this is the second point on the track
			// position the start flag at the midpoint
			if(i == 1)
			{
				StartFlagMeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(MidPoint, Rotation);
				StartFlagMeshActor->SetMobility(EComponentMobility::Stationary);

				UStaticMeshComponent* StartFlagMeshComponent = StartFlagMeshActor->GetStaticMeshComponent();
				if (StartFlagMeshComponent)
				{
					StartFlagMeshComponent->SetStaticMesh(StartFlagMesh);
				}
				
			}
		}
		PrevPosition = CurrentPosition;
	}
}

void AProceduralRacetrackActor::ClearTrack()
{
	for (auto MeshActor : RoadMeshActors)
	{
		if(MeshActor)
		{
			MeshActor->Destroy();
		}
	}

	for (auto MeshActor : TreeMeshesActors)
	{
		if(MeshActor)
		{
			MeshActor->Destroy();
		}
	}
	if(StartFlagMeshActor)
	{
		StartFlagMeshActor->Destroy();
	}
	RoadMeshActors.Empty();
	TreeMeshesActors.Empty();
	Vertices.Empty();
	Waypoints.Empty();
	Track.Empty();
}

void AProceduralRacetrackActor::GenerateTreeSpawnPositions()
{
	
}



void AProceduralRacetrackActor::SpawnTrees()
{
	TArray<FVector> PossibleSpawnPositions;
	for (auto Index : CentralIndexes)
	{
		// create an array of possible spawn positions using the waypoints list
		// and removing the race track positions from it
		if(!Track.Contains(Waypoints[Index]))
		{
			PossibleSpawnPositions.Add(Waypoints[Index]);
		}
	}
	
	// loop a set amount of times
	for(int32 i = 0; i <= TreeAmount; i++)
	{
		if(TreeMeshes.IsEmpty() || PossibleSpawnPositions.IsEmpty())
		{
			break;
		}
		// randomly select a tree from the list of tree meshes
		UStaticMesh* Tree = TreeMeshes[FMath::RandRange(0, TreeMeshes.Num() - 1)];
		FVector SpawnPosition = PossibleSpawnPositions[FMath::RandRange(0, PossibleSpawnPositions.Num() - 1)];
		
		// and spawn it in a random possible spawn location
		AStaticMeshActor* TreeMeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(SpawnPosition,
			FRotator(0,FMath::RandRange(0, 360),0));
		TreeMeshActor->SetMobility(EComponentMobility::Stationary);
		//TreeMeshActor->SetActorLocation(SpawnPosition);
		UStaticMeshComponent* TreeMeshComponent = TreeMeshActor->GetStaticMeshComponent();
		if (TreeMeshComponent)
		{
			TreeMeshComponent->SetStaticMesh(Tree);
		}
		TreeMeshesActors.Add(TreeMeshActor);
		// then remove that location from the possible spawns
		PossibleSpawnPositions.Remove(SpawnPosition);
	}
}

	
FVector AProceduralRacetrackActor::GetPointOnEdge(int32 EdgeIndex)
{
	FVector Position;
	if(EdgeIndex == 1)
	{
		// select random node on bottom edge
		Position = Waypoints[BottomEdgeIndexes[FMath::RandRange(0,BottomEdgeIndexes.Num() - 1)]];
	}
	if(EdgeIndex == 2)
	{
		// select random node on left edge
		Position = Waypoints[LeftEdgeIndexes[FMath::RandRange(0,LeftEdgeIndexes.Num() - 1)]];
	}
	if(EdgeIndex == 3)
	{
		// select random node on top edge
		Position = Waypoints[TopEdgeIndexes[FMath::RandRange(0,TopEdgeIndexes.Num() - 1)]];
	}
	if(EdgeIndex == 4)
	{
		// select random node on right edge
		Position = Waypoints[RightEdgeIndexes[FMath::RandRange(0,RightEdgeIndexes.Num() - 1)]];
	}
	return Position;
}


