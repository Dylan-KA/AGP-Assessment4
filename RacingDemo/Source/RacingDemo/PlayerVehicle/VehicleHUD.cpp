// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleHUD.h"
#include "Widgets/SWidget.h"

void UVehicleHUD::SetSpeedText(int32 NewSpeed)
{
	SpeedText->SetText(FText::FromString(FString::FromInt(NewSpeed)));
}

void UVehicleHUD::SetGearText(int32 NewGear)
{
	GearText->SetText(FText::FromString(FString::FromInt(NewGear)));
}

void UVehicleHUD::SetFuelText(float NewFuel)
{
	FuelText->SetText(FText::FromString(FString::Printf(TEXT("%.2f"), NewFuel)));
}

// Sets text to red if fuel empty and white otherwise
void UVehicleHUD::SetFuelColour(bool OutOfFuel)
{
	if (OutOfFuel)
	{
		// Set text to Red
		FLinearColor RedColour(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
		FuelText->SetColorAndOpacity(RedColour);
	} else
	{
		// Set text to White
		FLinearColor WhiteColour(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		FuelText->SetColorAndOpacity(WhiteColour);
	}
}
