// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Editor/EditorEngine.h" 
#include "Editor.h"
#include "LevelEditor.h"
#include "LevelEditorViewport.h"
#include "EditorViewportClient.h"
#include "UnrealClient.h"
#include "Kismet/KismetMathLibrary.h" 


#include "Math/UnrealMathUtility.h" 
#include "UObject/ConstructorHelpers.h" 
#include "MyParticleSystemActor.generated.h"

class Particle {
public:
	double Age;
	double MaxAge;
	FVector InitialVelocity;
	UStaticMeshComponent* ParticleMesh;
	FVector InitialLocation;
	

	Particle(double MaxAge, FVector InitialVelocity, UStaticMeshComponent* ParticleMesh, FVector InitialLocation)
		:Age(0), MaxAge(MaxAge), InitialVelocity(InitialVelocity), ParticleMesh(ParticleMesh), InitialLocation(InitialLocation)
	{
	}

	void Tick(double DeltaTime) {
		Age += DeltaTime;

		//update location
		auto new_location = FVector::ZeroVector;
		new_location.X = (InitialVelocity.X * Age) + InitialLocation.X;
		new_location.Y = (InitialVelocity.Y * Age) + InitialLocation.Y;
		new_location.Z = (-15 * Age * Age + InitialVelocity.Z * Age) + InitialLocation.Z;
		//ParticleMesh->SetRelativeLocation(new_location);
		ParticleMesh->SetWorldLocation(new_location);

		//update rotation
		FVector viewPos = FVector::ZeroVector;

		FViewport* activeViewport = GEditor->GetActiveViewport();
		FEditorViewportClient* editorViewClient = (activeViewport != nullptr) ? (FEditorViewportClient*)activeViewport->GetClient() : nullptr;
		if (editorViewClient)
		{
			viewPos = editorViewClient->GetViewLocation();
		}


		auto rotation = UKismetMathLibrary::FindLookAtRotation(viewPos, ParticleMesh->GetComponentLocation());
		rotation.Pitch += 90;
		ParticleMesh->SetRelativeRotation(rotation);
	}
};

UCLASS()
class RG_LAB3_API AMyParticleSystemActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyParticleSystemActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual bool ShouldTickIfViewportsOnly() const override;

private:
	UStaticMesh* MeshAsset;
	UMaterial* MeshMaterial;
	Particle* SpawnParticle();
	TSet<Particle*> Particles;
	int MaxParticles = 300;
	int MaxParticlesPerIteration = 15;
};
