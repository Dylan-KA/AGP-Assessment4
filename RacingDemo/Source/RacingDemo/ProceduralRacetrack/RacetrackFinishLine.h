// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "RacetrackFinishLine.generated.h"

UCLASS()
class RACINGDEMO_API ARacetrackFinishLine : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARacetrackFinishLine();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// When overlapped by the player, Update UI and Restart the Game
	UFUNCTION()
	virtual void OnFinishOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitInfo);
	
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* FinishCollider;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
