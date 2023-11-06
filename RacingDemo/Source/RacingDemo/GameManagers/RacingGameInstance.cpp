 // Fill out your copyright notice in the Description page of Project Settings.

#include "RacingGameInstance.h"

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


