// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "VehicleHUD.generated.h"

/**
 * 
 */
UCLASS()
class RACINGDEMO_API UVehicleHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetSpeedText(int32 NewSpeed);
	void SetGearText(int32 NewGear);
	void SetFuelText(float NewFuel);
	void SetFuelColour(bool OutOfFuel);
	void UpdateRaceTimer(int32 NewMinutes, int32 NewSeconds);
	void UpdateRestartTimer(int32 RestartSeconds);
	
protected:

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* SpeedText;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* GearText;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* FuelText;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* MinutesText;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* SecondsText;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* RestartText;
};
