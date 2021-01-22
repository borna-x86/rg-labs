// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


//#include "Plugins/Experimental/MeshModelingToolset/Source/ModelingComponents/Public/SimpleDynamicMeshComponent.h"

#include "Components/LineBatchComponent.h"
#include "ProceduralMeshComponent.h" 
#include "GameFramework/Actor.h"
#include "MyFunctionPlot.generated.h"

UENUM()
enum class EOptimizationFunction
{
	BasicParabola,
	RosenbrockBannana,
	SchafferF6
};

UCLASS()
class RG_LAB3_API AMyFunctionPlot : public AActor
{
	GENERATED_BODY()
	
	

	bool bBodyDidCompute = false;

	//function plot mesh
	TArray<FVector> verts;
	TArray<int32> inds;
	TArray<FLinearColor> VertexColors;

	//simplex
	TArray<FVector> Simplex;
	FVector InitialPoint;
	double InitialDist = 1.0;
	double Alpha = 1.0;
	double Beta = 0.5;
	double Gamma = 2.0;
	double Epsilon = 1e-3;


	//animation
	double TimeSinceLastRedraw = 0.0f;
	UProceduralMeshComponent* PMC;

	double (*fn)(double a, double b);

public:
	UPROPERTY(EditAnywhere)
	UMaterial* VertexMaterial;

	UPROPERTY(Category = Optimization, EditAnywhere)
	EOptimizationFunction GoalFunction;

	/*
		Which world coordinates corespond to the centerpoint of the plot.

		Eg. parabola (x - 2)**2 + (y - 3)**2 with ArgumentsAtCenter (2, 3) will "look like" x**2 + y**2
	*/
	UPROPERTY(Category = Optimization, EditAnywhere)
	FVector2D PlotArgumentsAtCenter = {0, 0};

	/*
		Range of plot
	*/
	UPROPERTY(Category = Optimization, EditAnywhere)
	FVector2D PlotFrom = -5;

	UPROPERTY(Category = Optimization, EditAnywhere)
	FVector2D PlotTo = 5;

	// Sets default values for this actor's properties
	AMyFunctionPlot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//double fn(double Valuex, double Valuey);
	TPair<int, int> _min_max_ind(TArray<FVector> vtxs);

	virtual bool ShouldTickIfViewportsOnly() const override;
	virtual void BeginDestroy() override;

	//UPROPERTY(VisibleAnywhere)
	//USimpleDynamicMeshComponent* MeshComponent = nullptr;

};
