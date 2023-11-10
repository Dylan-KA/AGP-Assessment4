 // Fill out your copyright notice in the Description page of Project Settings.

#include "RacingGameInstance.h"
#include "Kismet/GameplayStatics.h"

 UClass* URacingGameInstance::GetVehicleClass() const
{
  return VehicleClass.Get();
}

UClass* URacingGameInstance::GetRoadMeshClass() const
 {
  return RoadSplineMeshClass.Get();
 }

 UClass* URacingGameInstance::GetVehiclePlayerController() const
 {
  return VehiclePlayerController.Get();
 }

 UClass* URacingGameInstance::GetRampClass() const
 {
  return RampClass.Get();
 }

 UClass* URacingGameInstance::GetFuelPickupClass() const
 {
  return FuelPickupClass.Get();
 }

 UClass* URacingGameInstance::GetFinishLineClass() const
 {
  return FinishLineClass.Get();
 }

 void URacingGameInstance::PlayPickupSound()
 {
  UGameplayStatics::PlaySound2D(GetWorld(), PickupSoundCue);
 }

 void URacingGameInstance::PlayRestartSound()
 {
  UGameplayStatics::PlaySound2D(GetWorld(), RestartSoundCue);
 }

 void URacingGameInstance::PlayCountdownSound()
 {
  UGameplayStatics::PlaySound2D(GetWorld(), CountdownSoundCue);
 }


