// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/World.h" 
#include "Components/LineBatchComponent.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Math/RotationMatrix.h"

#include <fstream>

#include "BSplineComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RG_LAB1_API UBSplineComponent : public UActorComponent
{
	GENERATED_BODY()

	TArray<FVector> control_points;
	FVector segment_for_param(float t, FVector r1, FVector r2, FVector r3, FVector r4);
	FVector segment_tangent_for_param(float t, FVector r1, FVector r2, FVector r3, FVector r4);
	FVector segment_dd_for_param(float t, FVector r1, FVector r2, FVector r3, FVector r4);

public:	
	// Sets default values for this component's properties
	UBSplineComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
