// Fill out your copyright notice in the Description page of Project Settings.

#include "PCGVehiclePawn.h"
#include "ChaosVehicleMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "UObject/Class.h"
#include "RacingDemo/GameManagers/RacingGameInstance.h"

// Sets default values
APCGVehiclePawn::APCGVehiclePawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	ProceduralComponent = CreateDefaultSubobject<UProceduralComponent>("Procedural Component");
	FuelComponent = CreateDefaultSubobject<UFuelComponent>("Fuel Component");
	MyVehicleMovementComponent = GetVehicleMovementComponent();
	SetupLightComponents();
	
	bReplicates = true;
	
}

void APCGVehiclePawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(APCGVehiclePawn, FuelComponent);
	DOREPLIFETIME(APCGVehiclePawn, VehicleRarity);
	DOREPLIFETIME(APCGVehiclePawn, VehicleStats);
	
}

// Called when the game starts or when spawned
void APCGVehiclePawn::BeginPlay()
{
	Super::BeginPlay();
	
	// Initial setup for the Server
	// Server is responsible for generating procedural elements which is replicated to clients
	if (GetNetMode() == NM_ListenServer)
	{
		UE_LOG(LogTemp, Warning, TEXT("NetMode is ListenServer"))
		ProceduralComponent->GenerateRandomVehicle(VehicleRarity, VehicleStats);
		ApplyWeightDistribution();
		FuelComponent->SetCurrentFuel(VehicleStats.MaxFuelCapacity);

		GenerateProceduralMaterial();
		SetUnderGlowColour();
	} else if (GetNetMode() == NM_DedicatedServer)
	{
		UE_LOG(LogTemp, Warning, TEXT("NetMode is DedicatedServer"))
		ProceduralComponent->GenerateRandomVehicle(VehicleRarity, VehicleStats);
		ApplyWeightDistribution();
		FuelComponent->SetCurrentFuel(VehicleStats.MaxFuelCapacity);
	}
	
	// Initial setup for standalone 
	if (GetNetMode() == NM_Standalone)
	{
		UE_LOG(LogTemp, Warning, TEXT("NetMode is Standalone"))
		ProceduralComponent->GenerateRandomVehicle(VehicleRarity, VehicleStats);
		ApplyWeightDistribution();
		GenerateProceduralMaterial();
		SetUnderGlowColour();
		FuelComponent->SetCurrentFuel(VehicleStats.MaxFuelCapacity);
		DrawUI();
	}
}

// Generates and applies a procedural material
// Colour is set to the rarity of the car
// other values are randomly chosen (roughness, metallic, specular)
void APCGVehiclePawn::GenerateProceduralMaterial()
{
	// Get the material for the chassis
	ProceduralMaterial = GetMesh()->GetMaterial(2);
	// Make a dynamic material instance from the material above
	UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(ProceduralMaterial, this);
	// Set the chassis material to this Dynamic material
	GetMesh()->SetMaterial(2, DynamicMaterial);
	// Randomise parameters
	DynamicMaterial->SetScalarParameterValue(TEXT("Roughness"),  FMath::RandRange(0.0f, 1.0f));
	DynamicMaterial->SetScalarParameterValue(TEXT("Metallic"), FMath::RandRange(0.0f, 1.0f));
	DynamicMaterial->SetScalarParameterValue(TEXT("Specular"), FMath::RandRange(0.0f, 1.0f));
	// Set Colour based on rarity
	switch (VehicleRarity)
	{
	case EVehicleRarity::Legendary:
		DynamicMaterial->SetVectorParameterValue(TEXT("Base Color"), FColor::Yellow);
		break;
	case EVehicleRarity::Master:
		DynamicMaterial->SetVectorParameterValue(TEXT("Base Color"), FColor(255, 0, 150));
		break;
	case EVehicleRarity::Rare:
		DynamicMaterial->SetVectorParameterValue(TEXT("Base Color"), FColor::Blue);
		break;
	case EVehicleRarity::Common:
		DynamicMaterial->SetVectorParameterValue(TEXT("Base Color"), FColor::White);
		break;
	default:
		break;
	}
}

// Initialises and attaches all light components to the vehicle
// also sets the position and intensity of all light components
void APCGVehiclePawn::SetupLightComponents()
{
	FrontLightComponent = CreateDefaultSubobject<UPointLightComponent>("Front Light");
	FrontLightComponent->SetupAttachment(GetRootComponent());
	FrontLightComponent->SetRelativeLocation(FVector(240,0,-10));
	FrontLightComponent->SetIntensity(10'000.0f);
	FrontLightComponent->CastShadows = false;
	
	BackLightComponent = CreateDefaultSubobject<UPointLightComponent>("Back Light");
	BackLightComponent->SetupAttachment(GetRootComponent());
	BackLightComponent->SetRelativeLocation(FVector(-200,0,-10));
	BackLightComponent->SetIntensity(10'000.0f);
	BackLightComponent->CastShadows = false;
	
	LeftLightComponent = CreateDefaultSubobject<UPointLightComponent>("Left Light");
	LeftLightComponent->SetupAttachment(GetRootComponent());
	LeftLightComponent->SetRelativeLocation(FVector(0,-100,-10));
	LeftLightComponent->SetIntensity(10'000.0f);
	LeftLightComponent->CastShadows = false;
	
	RightLightComponent = CreateDefaultSubobject<UPointLightComponent>("Right Light");
	RightLightComponent->SetupAttachment(GetRootComponent());
	RightLightComponent->SetRelativeLocation(FVector(0,100,-10));
	RightLightComponent->SetIntensity(10'000.0f);
	RightLightComponent->CastShadows = false;
}

// Sets the colour of the light underneath the car based on rarity
void APCGVehiclePawn::SetUnderGlowColour() const
{
	if (FrontLightComponent && BackLightComponent && LeftLightComponent && RightLightComponent)
	{
		switch (VehicleRarity)
		{
		case EVehicleRarity::Legendary:
			FrontLightComponent->SetLightColor(FColor::Yellow);
			BackLightComponent->SetLightColor(FColor::Yellow);
			LeftLightComponent->SetLightColor(FColor::Yellow);
			RightLightComponent->SetLightColor(FColor::Yellow);
			break;
		case EVehicleRarity::Master:
			FrontLightComponent->SetLightColor(FColor(255, 0, 150));
			BackLightComponent->SetLightColor(FColor(255, 0, 150));
			LeftLightComponent->SetLightColor(FColor(255, 0, 150));
			RightLightComponent->SetLightColor(FColor(255, 0, 150));
			break;
		case EVehicleRarity::Rare:
			FrontLightComponent->SetLightColor(FColor::Blue);
			BackLightComponent->SetLightColor(FColor::Blue);
			LeftLightComponent->SetLightColor(FColor::Blue);
			RightLightComponent->SetLightColor(FColor::Blue);
			break;
		case EVehicleRarity::Common:
			FrontLightComponent->SetLightColor(FColor::White);
			BackLightComponent->SetLightColor(FColor::White);
			LeftLightComponent->SetLightColor(FColor::White);
			RightLightComponent->SetLightColor(FColor::White);
			break;
		default:
			break;
		}
	}
	else
	{
		// Handle null pointers or other issues here
		UE_LOG(LogTemp, Error, TEXT("One or more light components are null in SetUnderGlowColour()"));
	}
}

void APCGVehiclePawn::DrawUI()
{
	if (IsLocallyControlled() && VehicleHUDClass)
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		{
			VehicleHUD = CreateWidget<UVehicleHUD>(PlayerController, VehicleHUDClass);
			if (VehicleHUD)
			{
				VehicleHUD->AddToPlayerScreen();
			}
		}
	}
	UpdateUI();
}

void APCGVehiclePawn::UpdateUI()
{
	if (IsLocallyControlled())
	{
		if (!VehicleHUD)
		{
			UE_LOG(LogTemp, Error, TEXT("VehicleHUD is NULL, attempting to create"));
			DrawUI();
			return;
		}
		
		if (MyVehicleMovementComponent->GetForwardSpeedMPH() >= 0)
		{
			VehicleHUD->SetSpeedText(MyVehicleMovementComponent->GetForwardSpeedMPH());
		} else
		{
			// Prevents returning a negative speed when reversing
			VehicleHUD->SetSpeedText(MyVehicleMovementComponent->GetForwardSpeedMPH() * -1);
		}
	
		if (MyVehicleMovementComponent->GetCurrentGear() >= 0)
		{
			VehicleHUD->SetGearText(MyVehicleMovementComponent->GetCurrentGear());
		} else
		{
			// Prevents returning a negative gear when reversing
			VehicleHUD->SetGearText(MyVehicleMovementComponent->GetCurrentGear() * -1);
		}

		VehicleHUD->SetFuelText(FuelComponent->GetCurrentFuel());
	}
}

void APCGVehiclePawn::ManageFuel(float DeltaTime)
{
	if (FuelComponent->IsOutOfFuel())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Out of fuel"))
		FuelComponent->SetCurrentFuel(0.0f);
	} else
	{
		FuelComponent->UpdateFuelAmount(DeltaTime);
	}
	
	if (VehicleHUD)
	{
		VehicleHUD->SetFuelColour(FuelComponent->IsOutOfFuel());
	}
}

// Called every frame
void APCGVehiclePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	ManageFuel(DeltaTime);
	
	UpdateUI();
	
	// Check if there is a Controller, if not then attempt to create a new one
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (GetController() == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Controller is null on AutonomousProxy, attempting to create a new one"))
			if (const URacingGameInstance* GameInstance = GetWorld()->GetGameInstance<URacingGameInstance>())
			{
				// Get blueprint implementation of 
				APlayerController* NewController = GetWorld()->SpawnActor<APlayerController>(GameInstance->GetVehiclePlayerController());
				if (NewController)
				{
					NewController->Possess(this);
				}
			}
		}
	}
	
}

// Called to bind functionality to input
void APCGVehiclePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
}

// Called when the rarity is changed and replicated to the client
void APCGVehiclePawn::SetVehicleVisuals()
{
	FText VehicleRarityText = UEnum::GetDisplayValueAsText(VehicleRarity);
	UE_LOG(LogTemp, Warning, TEXT("RARITY REPLICATED ON CLIENT: %s"), *VehicleRarityText.ToString())

	GenerateProceduralMaterial();
	SetUnderGlowColour();
	
	DrawUI();
}

// Called when the VehicleStats is changed and replicated to the client
void APCGVehiclePawn::SetVehicleProcedural()
{
	UE_LOG(LogTemp, Warning, TEXT("VEHICLE STATS REPLICATED ON CLIENT: %s"), *VehicleStats.ToString())
	ApplyWeightDistribution();
	FuelComponent->SetCurrentFuel(VehicleStats.MaxFuelCapacity);
}
