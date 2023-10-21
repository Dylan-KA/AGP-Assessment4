// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleHUD.h"

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