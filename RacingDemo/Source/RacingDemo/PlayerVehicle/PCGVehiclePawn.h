// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FuelComponent.h"
#include "WheeledVehiclePawn.h"
#include "UProceduralComponent.h"
#include "Components/PointLightComponent.h"
#include "VehicleHUD.h"
#include "PCGVehiclePawn.generated.h"

USTRUCT(BlueprintType)
struct FVehicleStats
{
	GENERATED_BODY()
public:

	// Acceleration is multiplied by the player's throttle inputs to provide different acceleration speeds
	UPROPERTY(BlueprintReadOnly)
	float Acceleration = 1.0f;

	// ReverseAcceleration is multiplied by the player's reverse inputs to provide different reverse speeds
	UPROPERTY(BlueprintReadOnly)
	float ReverseAcceleration = 1.0f;
	
	// TopSpeed represents the Max RPM which us used in vehicle movement's physics
	UPROPERTY(BlueprintReadOnly)
	float TopSpeed = 7500;

	// WeightDistribution offsets the centre of mass of the vehicle along the x-axis (forwards/backwards)
	UPROPERTY(BlueprintReadOnly)
	float WeightDistribution = 0;

	// MaxFuel is the amount of distance that the vehicle can drive
	UPROPERTY(BlueprintReadOnly)
	float MaxFuelCapacity = 10.0f;

	// TurningSpeed is multiplied by the player's steering inputs to provide different reverse speeds
	UPROPERTY(BlueprintReadOnly)
	float TurningSpeed = 1.0f;

	/**
	 * A debug ToString function that allows the easier printing of the vehicle stats.
	 * @return A string detailing the vehicle stats stored in this struct.
	 */
	FString ToString() const
	{
		FString WeaponString = "\n";
		WeaponString += "Acceleration: " + FString::SanitizeFloat(Acceleration) + "\n";
		WeaponString += "ReverseAcceleration: " + FString::SanitizeFloat(ReverseAcceleration)  + "\n";
		WeaponString += "TopSpeed (Max RPM): " + FString::FromInt(TopSpeed) + "\n";
		WeaponString += "WeightDistribution (Positive to Front / Negative to Rear): " + FString::SanitizeFloat(WeightDistribution) + "\n";
		WeaponString += "MaxFuelCapacity (Litres): " + FString::SanitizeFloat(MaxFuelCapacity) + "\n";
		WeaponString += "TurningSpeed: " + FString::SanitizeFloat(TurningSpeed) + "\n";
		return WeaponString;
	}
};

UCLASS()
class RACINGDEMO_API APCGVehiclePawn : public AWheeledVehiclePawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APCGVehiclePawn();

	// Current stats of the vehicle
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = SetVehicleProcedural)
	FVehicleStats VehicleStats;

	// Vehicle HUD
	UPROPERTY()
	UVehicleHUD* VehicleHUD;

	// Current rarity of the vehicle
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = SetVehicleVisuals)
	EVehicleRarity VehicleRarity;

	// FuelComponent
	UPROPERTY(BlueprintReadOnly, Replicated)
	UFuelComponent* FuelComponent;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// ProceduralComponent is responsible for generating the rarity and stats of the vehicle
	UPROPERTY(Replicated)
	UProceduralComponent* ProceduralComponent;
	
	// ChaosVehicleMovementComponent used for getting current speed of vehicle
	UPROPERTY()
	UChaosVehicleMovementComponent* MyVehicleMovementComponent;

	// Procedural Vehicle Material
	UPROPERTY(VisibleAnywhere)
	UMaterialInterface* ProceduralMaterial;

	// Generates and applies a procedural material
	// Colour is set to the rarity of the car
	// other values are randomly chosen (roughness, metallic, specular)
	void GenerateProceduralMaterial();
	
	// Under-glow light components
	UPROPERTY(VisibleDefaultsOnly)
	UPointLightComponent* FrontLightComponent;
	UPROPERTY(VisibleDefaultsOnly)
	UPointLightComponent* BackLightComponent;
	UPROPERTY(VisibleDefaultsOnly)
	UPointLightComponent* LeftLightComponent;
	UPROPERTY(VisibleDefaultsOnly)
	UPointLightComponent* RightLightComponent;
	
	// Initialises and attaches all light components to the vehicle
	// also sets the position and intensity of all light components
	void SetupLightComponents();
	
	// Sets the colour of the light underneath the car based on rarity
	void SetUnderGlowColour() const;

	// Vehicle HUD Class
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UVehicleHUD> VehicleHUDClass;

	// Vehicle HUD
	UFUNCTION()
	void DrawUI();
	UFUNCTION()
	void UpdateUI();

	// Called during tick function to handle empty fuel
	void ManageFuel(float DeltaTime);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Blueprint event which sets the CentreOfMassOffset for the mesh
	UFUNCTION(BlueprintImplementableEvent, Category = "PCG Vehicle Pawn")
	void ApplyWeightDistribution();

private:

	UFUNCTION()
	void SetVehicleVisuals();

	UFUNCTION()
	void SetVehicleProcedural();
	
};
