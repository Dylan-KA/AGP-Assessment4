// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleHUD.h"
#include "Widgets/SWidget.h"

void UVehicleHUD::SetSpeedText(int32 NewSpeed)
{
	if (SpeedText)
	{
		SpeedText->SetText(FText::FromString(FString::FromInt(NewSpeed)));
	}
}

void UVehicleHUD::SetGearText(int32 NewGear)
{
	if (GearText)
	{
		GearText->SetText(FText::FromString(FString::FromInt(NewGear)));
	}
}

void UVehicleHUD::SetFuelText(float NewFuel)
{
	if (FuelText)
	{
		FuelText->SetText(FText::FromString(FString::Printf(TEXT("%.2f"), NewFuel)));
	}
}

// Sets text to red if fuel empty and white otherwise
void UVehicleHUD::SetFuelColour(bool OutOfFuel)
{
	if (FuelText)
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
}

void UVehicleHUD::UpdateRaceTimer(int32 NewMinutes, int32 NewSeconds)
{
	MinutesText->SetText(FText::FromString(FString::FromInt(NewMinutes)));
	SecondsText->SetText(FText::FromString(FString::FromInt(NewSeconds)));
}

void UVehicleHUD::UpdateRestartTimer(int32 RestartSeconds)
{
	if (RestartSeconds != -1)
	{
		RestartText->SetText(FText::FromString(FString::FromInt(RestartSeconds)));
	} else
	{
		RestartText->SetText(FText::GetEmpty());
	}
}
