// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralRacetrackActor.h"
#include "ProceduralMeshComponent.h"
#include "RacingDemo/GameManagers/RacingGameInstance.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "RacingDemo/Pickups/FuelPickup.h"
#include "RacingDemo/GameManagers/MyRacingGameMode.h"

// Sets default values
AProceduralRacetrackActor::AProceduralRacetrackActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;
	
	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Procedural Mesh"));
	SetRootComponent(ProceduralMesh);
	
}

void AProceduralRacetrackActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AProceduralRacetrackActor, Track); 
	DOREPLIFETIME(AProceduralRacetrackActor, TreeValues); 
}

FTrackSection AProceduralRacetrackActor::GetTrackStart() const
{
	return Track[0]; 
}

FTrackSection AProceduralRacetrackActor::GetTrackEnd() const
{
	// return second last section as the end of the track
	return Track[Track.Num()-2]; 
}


bool AProceduralRacetrackActor::HasGenerated() const
{
	return bHasGenerated;
}

// Called when the game starts or when spawned
void AProceduralRacetrackActor::BeginPlay()
{
	Super::BeginPlay();
	PathfindingSubsystem = GetWorld()->GetSubsystem<UPathfindingSubsystem>();
	GameInstance = GetWorld()->GetGameInstance<URacingGameInstance>();
	if(PathfindingSubsystem && GameInstance)
	{
		GenerateRacetrackLevel(); 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find subsystems"))
	}
}

void AProceduralRacetrackActor::GenerateRacetrackLevel()
{
	ClearTrackMeshes();
	// if on the server generate the racetrack level details
	// then replicate them to the client
	if(GetNetMode() < ENetMode::NM_Client)
	{
		ClearTrackValues(); 
		GenerateGrid();
		InitialiseIndexes();
		RandomiseStartAndEnd();
		RandomiseCheckpoint();
		GenerateTrack();
		GenerateTrees();
		SpawnTrack();
		SpawnTrees();
		SpawnFuelPickups();
		SpawnRamps();
		SpawnFinishLine();
	}
	
}

// Called every frame
void AProceduralRacetrackActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
	// Ignore the left and right rows
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
	
	// find all central indexes of the grid by skipping thw top and bottom rows
	for(int32 i = Width; i < (Width * Height) - (Width * 2); i++)
	{
		// check if index is on the left or right edge
		if(LeftEdgeIndexes.Contains(i) || RightEdgeIndexes.Contains(i))
		{
			continue;
		}
		CentralIndexes.Add(i);
	}
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
	// Get random point along both selected edges
	StartPosition = GetPointOnEdge(StartEdge);
	EndPosition = GetPointOnEdge(EndEdge);
}

void AProceduralRacetrackActor::RandomiseCheckpoint()
{
	// get two random central checkpoints
	int32 CheckpointIndex1 = CentralIndexes[FMath::RandRange(0, CentralIndexes.Num() - 1)];
	Checkpoint1 = Waypoints[CheckpointIndex1];
	CentralIndexes.Remove(CheckpointIndex1);
	int32 CheckpointIndex2 = CentralIndexes[FMath::RandRange(0, CentralIndexes.Num() - 1)];
	Checkpoint2 = Waypoints[CheckpointIndex2];
}

void AProceduralRacetrackActor::GenerateTrack()
{
	// generate a path between all checkpoints
	TArray<FVector> Path; 
	Path = PathfindingSubsystem->GetPath(StartPosition, Checkpoint1);
	Path.Pop(); 
	Path.Append(PathfindingSubsystem->GetPath(Checkpoint1, Checkpoint2));
	Path.Pop(); 
	Path.Append(PathfindingSubsystem->GetPath(Checkpoint2, EndPosition));
	//store path positions in track
	for (auto Position : Path)
	{
		// store the path in a list of track sections
		FTrackSection Section;
		Section.Position = Position; 
		Track.Add(Section);
	}
}

void AProceduralRacetrackActor::SpawnTrack()
{
	FVector PrevPosition;
	// spawn in a road mesh for every section of the track  
	for (int32 i = 0; i < Track.Num(); i++)
	{
		FVector CurrentPosition = Track[i].Position;
		// if this is the second track section 
		if( i != 0 )
		{
			// Determine the midpoint between sections for item spawning
			FVector MidPoint = (CurrentPosition + PrevPosition) / 2;
			Track[i-1].Midpoint = MidPoint;
			// determine difference between positions so splines can be adjusted
			FVector Diff = CurrentPosition - PrevPosition;
			PrevPosition.Z += 1;
			// Find the orientation of each track section
			FRotator Rotation = FRotator(0,UKismetMathLibrary::FindLookAtRotation(PrevPosition, CurrentPosition).Yaw,0);
			Track[i-1].ForwardRotation = Rotation;
			// Spawn road splines
			ARoadSplineMeshActor* RoadMeshActor = GetWorld()->SpawnActor<ARoadSplineMeshActor>(
			GameInstance->GetRoadMeshClass(), PrevPosition,Rotation);
			// set the starting and ending positions of each spline at the position of the next section 
			RoadMeshActor->GetSplineMeshComponent()->SetStartPosition(FVector(-200 ,0.0, 1.0 ));
			RoadMeshActor->GetSplineMeshComponent()->SetEndPosition(FVector(Diff.Length() + 200,0.0, 1.0 ));
			RoadMeshActors.Add(RoadMeshActor);
		}
		PrevPosition = CurrentPosition;
	}
	SpawnStartLine();

	bHasGenerated = true;
}

void AProceduralRacetrackActor::ClearTrackMeshes()
{
	// clear all spawned meshes
	for (auto MeshActor : RoadMeshActors)
	{
		if(MeshActor)
		{
			MeshActor->Destroy();
		}
	}
	for (auto MeshActor : TreeMeshActors)
	{
		if(MeshActor)
		{
			MeshActor->Destroy();
		}
	}
	for (auto Pickup : FuelPickups)
	{
		if(Pickup)
		{
			Pickup->Destroy();
		}
	}
	if(StartFlagMeshActor)
	{
		StartFlagMeshActor->Destroy();
		StartFlagMeshActor = nullptr;
	}
	
	RoadMeshActors.Empty();
	TreeMeshActors.Empty();
	FuelPickups.Empty();
}

void AProceduralRacetrackActor::ClearTrackValues()
{
	// clear generated track values
	Vertices.Empty();
	Waypoints.Empty();
	Track.Empty();
	TreeValues.Empty();
}

void AProceduralRacetrackActor::GenerateTrees()
{
	TArray<FVector> PossibleSpawnPositions;

	for (auto Index : CentralIndexes)
	{
		// create an array of possible spawn positions using the waypoints list
		PossibleSpawnPositions.Add(Waypoints[Index]);
	}
	// removing the race track positions from possible spawn positions
	for (auto TrackSection : Track)
	{
		PossibleSpawnPositions.Remove(TrackSection.Position); 
	}

	// loop a set amount of times
	for(int32 i = 0; i <= TreeAmount; i++)
	{
		if(TreeMeshes.IsEmpty() || PossibleSpawnPositions.IsEmpty())
		{
			break;
		}
		// randomly select a position, rotation and mesh for each tree
		FVector Position = PossibleSpawnPositions[FMath::RandRange(0, PossibleSpawnPositions.Num() - 1)];
		FTree Tree = FTree();
		Tree.Position = Position;
		Tree.Rotation = FRotator(0,FMath::RandRange(0, 360),0);
		Tree.MeshIndex = FMath::RandRange(0, TreeMeshes.Num() - 1);
		Tree.Scale = FMath::RandRange(1.0f, 1.75f);
		// Add each set of tree values to array
		TreeValues.Add(Tree);

		// then remove that location from the possible spawns
		PossibleSpawnPositions.Remove(Position);
	}
}




void AProceduralRacetrackActor::SpawnTrees()
{
	for (auto TreeValue : TreeValues)
	{
		// Spawn in each tree
		AStaticMeshActor* TreeMeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(TreeValue.Position,
			TreeValue.Rotation);
		TreeMeshActor->SetMobility(EComponentMobility::Stationary);
		UStaticMeshComponent* TreeMeshComponent = TreeMeshActor->GetStaticMeshComponent();
		TreeMeshActor->SetActorScale3D(FVector(TreeValue.Scale, TreeValue.Scale, TreeValue.Scale));
		if (TreeMeshComponent)
		{
			TreeMeshComponent->SetStaticMesh(TreeMeshes[TreeValue.MeshIndex]);
			TreeMeshComponent->SetCollisionProfileName("BlockAll");
		}
		TreeMeshActors.Add(TreeMeshActor);
	}
}

void AProceduralRacetrackActor::SpawnFuelPickups()
{
	// add track positions to the possible spawns list
	// Ignore the first and last two points
	for (int i = 2; i < Track.Num() - 2; i++)
	{
		PossibleTrackSpawnPositions.Add(Track[i]);
	}
	// Number of fuel pickups is  dependent on length of the track
	for(int i = 0; i < Track.Num()/10; i++)
	{
		//Randomly select track section
		FTrackSection TrackSection = PossibleTrackSpawnPositions[FMath::RandRange(0,PossibleTrackSpawnPositions.Num()-1)];
		FVector CentreSpawnPosition = TrackSection.Midpoint;
		// ensure spawn position is above the track
		CentreSpawnPosition.Z += 100;
		// find positions adjacent to centre spawn position
		FVector LeftSpawnPosition = GetRelativePosition(CentreSpawnPosition, TrackSection.ForwardRotation, "Left");
		FVector RightSpawnPosition = GetRelativePosition(CentreSpawnPosition, TrackSection.ForwardRotation, "Right");
		//Spawn 3 fuel pickups in a row at selected track section
		AFuelPickup* FuelPickupCentre = GetWorld()->SpawnActor<AFuelPickup>(
		GameInstance->GetFuelPickupClass(), CentreSpawnPosition, FRotator(0,0,0));
		AFuelPickup* FuelPickupLeft = GetWorld()->SpawnActor<AFuelPickup>(
		GameInstance->GetFuelPickupClass(), LeftSpawnPosition, FRotator(0,0,0));
		AFuelPickup* FuelPickupRight = GetWorld()->SpawnActor<AFuelPickup>(
		GameInstance->GetFuelPickupClass(), RightSpawnPosition, FRotator(0,0,0));
		
		FuelPickups.Add(FuelPickupCentre);
		FuelPickups.Add(FuelPickupLeft);
		FuelPickups.Add(FuelPickupRight);
		//Remove position from list
		PossibleTrackSpawnPositions.Remove(TrackSection);
	}
}

void AProceduralRacetrackActor::SpawnRamps()
{
	for(int i = 0; i < Track.Num()/8; i++)
	{
		FVector SpawnPosition;
		int32 RandomDirection = FMath::RandRange(1,3);
		FTrackSection TrackSection = PossibleTrackSpawnPositions[FMath::RandRange(0,PossibleTrackSpawnPositions.Num()-1)];
		FVector CentrePosition = TrackSection.Midpoint;
		// select a random track section and alignment as a spawn position
		switch (RandomDirection)
		{
		case 1:
			SpawnPosition = CentrePosition;
			break;
		case 2:
			SpawnPosition = GetRelativePosition(CentrePosition, TrackSection.ForwardRotation, "Right");
			break;
		case 3:
			SpawnPosition = GetRelativePosition(CentrePosition, TrackSection.ForwardRotation, "Left");
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("Invalid Direction"));
			break;
		}
		// spawn the ramp actor
		ARampActor* RampActor = GetWorld()->SpawnActor<ARampActor>(GameInstance->GetRampClass(), SpawnPosition, TrackSection.ForwardRotation);
		Ramps.Add(RampActor);
		PossibleTrackSpawnPositions.Remove(TrackSection);
	}
}

void AProceduralRacetrackActor::SpawnStartLine()
{
	StartFlagMeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(Track[1].Midpoint, Track[1].ForwardRotation);
	StartFlagMeshActor->SetMobility(EComponentMobility::Stationary);
	StartFlagMeshActor->SetActorScale3D(FVector(1,1.5,1));
	UStaticMeshComponent* StartFlagMeshComponent = StartFlagMeshActor->GetStaticMeshComponent();
	if (StartFlagMeshComponent)
	{
		StartFlagMeshComponent->SetStaticMesh(StartFlagMesh);
		StartFlagMeshComponent->SetCollisionProfileName("BlockAll");
	}
}

void AProceduralRacetrackActor::SpawnFinishLine()
{
	ARacetrackFinishLine* FinishLine = GetWorld()->SpawnActor<ARacetrackFinishLine>(
		GameInstance->GetFinishLineClass(), GetTrackEnd().Position, GetTrackEnd().ForwardRotation);
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

void AProceduralRacetrackActor::PrintTrack()
{
	UE_LOG(LogTemp, Warning, TEXT("PRINTING TRACK"));
	for (auto TrackSection : Track)
	{
		UE_LOG(LogTemp, Warning, TEXT("STEP"));
		UE_LOG(LogTemp, Warning, TEXT("Position: %s"), *TrackSection.Position.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Rotation: %s"), *TrackSection.ForwardRotation.ToString());
	}
}

FVector AProceduralRacetrackActor::GetRelativePosition(FVector Position, FRotator ForwardRotation, FString Direction)
{
	FVector Rotation;
	// Get the rotation vector and rotate it 90 degrees left or right around the Z axis
	if(Direction.Equals("Left"))
	{
		Rotation = ForwardRotation.Vector().RotateAngleAxis(-90, FVector(0,0,1));
	}
	if(Direction.Equals("Right"))
	{
		Rotation = ForwardRotation.Vector().RotateAngleAxis(90, FVector(0,0,1));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Direction"));
	}
	return Position + Rotation * 500;
}


