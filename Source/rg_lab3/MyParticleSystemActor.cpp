// Fill out your copyright notice in the Description page of Project Settings.


#include "MyParticleSystemActor.h"



// Sets default values
AMyParticleSystemActor::AMyParticleSystemActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	auto DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAssetCtor(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	ConstructorHelpers::FObjectFinder<UMaterial> MeshMaterialCtor(TEXT("Material'/Game/particle_textures/balloon_Mat.balloon_Mat'"));

	MeshAsset = MeshAssetCtor.Object;
	MeshMaterial = MeshMaterialCtor.Object;


}

Particle* AMyParticleSystemActor::SpawnParticle() {
	auto Mesh = NewObject<UStaticMeshComponent>(this);
	
	Mesh->AttachTo(RootComponent);
	Mesh->SetStaticMesh(MeshAsset);
	Mesh->SetMaterial(0, MeshMaterial);
	Mesh->SetVisibility(true);
	Mesh->SetRelativeScale3D({ 0.1, 0.1, 0.1f });
	Mesh->SetRelativeLocation(FVector(0.f, 0.f, 0.f));


	Mesh->RegisterComponent();
	
	auto v1 = FMath::RandRange(10.0f, 40.0f);
	v1 *= (FMath::RoundToInt(v1) % 2 == 0 ? -1 : 1);

	auto v2 = FMath::RandRange(10.0f, 40.0f);
	v2 *= (FMath::RoundToInt(v2) % 2 == 0 ? -1 : 1);

	return new Particle(FMath::RandRange(1.5f, 10.0f), { v1, v2, FMath::RandRange(40.0f, 90.0f) }, Mesh, GetActorLocation());

}

// Called when the game starts or when spawned
void AMyParticleSystemActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMyParticleSystemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//spawn new particles, if needed
	int particles_spawned = 0;
	while (particles_spawned < MaxParticlesPerIteration && Particles.Num() < MaxParticles) {
		Particles.Add(SpawnParticle());
		particles_spawned++;
	}

	TArray<Particle*> toDestroy;
	for (auto& particle : Particles) {
		particle->Tick(DeltaTime);

		if (particle->Age > particle->MaxAge) {
			particle->ParticleMesh->DestroyComponent();
			toDestroy.Add(particle);
		}
		
	}

	for (auto particle : toDestroy) {
		Particles.Remove(particle);
	}

	toDestroy.Reset();

}


bool AMyParticleSystemActor::ShouldTickIfViewportsOnly() const {
	return true;
}
