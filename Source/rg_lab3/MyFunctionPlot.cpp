// Fill out your copyright notice in the Description page of Project Settings.


#include "MyFunctionPlot.h"

// Sets default values
AMyFunctionPlot::AMyFunctionPlot()
{

	switch (GoalFunction) {
	case(EOptimizationFunction::RosenbrockBannana): 
	{
		fn = [](double Valuex, double Valuey)
		{

			//return Valuex * Valuex + Valuey * Valuey;
			return 100 * pow(Valuey - Valuex * Valuex, 2)
				+ pow((1 - Valuex), 2);

		};
	}

	case(EOptimizationFunction::BasicParabola):
		fn = [](double Valuex, double Valuey)
		{
			return 50*Valuex * Valuex + 50*Valuey * Valuey;
		};

	case(EOptimizationFunction::SchafferF6):
		fn = [](double Valuex, double Valuey)
		{
			Valuex *= 5;
			Valuey *= 5;

			return 500*(0.5 + (pow(sin(sqrt(Valuex * Valuex + Valuey * Valuey)), 2) - 0.5) / pow(1 + 0.001 * (Valuex * Valuex + Valuey * Valuey),2));
		};
	}

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	PMC = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("PMC"));
	VertexMaterial = CreateDefaultSubobject<UMaterial>(TEXT("VertexMaterial"));
	RootComponent = PMC;

	InitialPoint.X = 3;
	InitialPoint.Y = 3;

	Simplex.Add(InitialPoint);

	for (auto i = 0u; i < 2; ++i) {
		FVector next;
		next = InitialPoint;
		next[i] += InitialDist;
		Simplex.Add(next);
	}

}

// Called when the game starts or when spawned
void AMyFunctionPlot::BeginPlay()
{
	Super::BeginPlay();
	
}

FVector _centroid(TArray<FVector> v, int ind_h) {
	FVector res(0, 0, 0);
	for (int i = 0u; i < v.Num(); ++i) {
		if (i == ind_h) continue;

		res += v[i];
	}

	return res / (v.Num() - 1.0);
}

FVector _reflect(FVector xc, FVector xh, double alpha) {
	return xc * (1 + alpha) - xh * alpha;
}

FVector _contract(FVector xc, FVector xh, double beta) {
	return xc * (1 - beta) + xh * beta;
}

FVector _expand(FVector xc, FVector xr, double gamma) {
	return _contract(xc, xr, gamma);
}

TPair<int, int> AMyFunctionPlot::_min_max_ind(TArray<FVector> vtxs) {
	int min_ind = 0;
	int max_ind = 0;
	double min = fn(vtxs[0].X, vtxs[0].Y);
	double max = fn(vtxs[0].X, vtxs[0].Y);

	for (auto i = 1; i < vtxs.Num(); ++i) {
		if (fn(vtxs[i].X, vtxs[i].Y) < min) {
			min = fn(vtxs[i].X, vtxs[i].Y);
			min_ind = i;
		}

		if (fn(vtxs[i].X, vtxs[i].Y) > max) {
			max = fn(vtxs[i].X, vtxs[i].Y);
			max_ind = i;
		}
	}

	return TPair<int, int>(min_ind, max_ind);
}

// Called every frame
void AMyFunctionPlot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TimeSinceLastRedraw += DeltaTime;

	if (!bBodyDidCompute) {
		bBodyDidCompute = true;

		double max_z = fn(-2000/1000, -2000/1000);
		double min_z = fn(-2000/1000, -2000/1000);

		const int points_per_row = 201;
		int32 k = 0;
		for (double i = -2000; i <= 2000; i += 20)
		{
			for (double j = -2000; j <= 2000; j += 20)
			{

				FVector pos;
				pos.X = i;
				pos.Y = j;
				pos.Z = fn(i / 400, j / 400);

				if (pos.Z > max_z) {
					max_z = pos.Z;
				}

				if (pos.Z < min_z) {
					min_z = pos.Z;
				}

				verts.Add(pos);

				if (j != 2000 && i != 2000) {
					inds.Add(k);
					inds.Add(k + 1);
					inds.Add(k + points_per_row);
					inds.Add(k + 1);
					inds.Add(k + 1 + points_per_row);
					inds.Add(k + points_per_row);
				}

				k++;
			}
		}

		//UE_LOG(LogTemp, Warning, TEXT("%f, %f, %f"), min_z, max_z, verts[5].Z);
		for (auto i = 0; i < verts.Num(); ++i) {
			auto red_value = (verts[i].Z - min_z) / (max_z - min_z);
			//UE_LOG(LogTemp, Warning, TEXT("kkz = %f, maxz = %f, minz = %f, red = %f"), verts[i].Z, max_z, min_z, red_value);

			VertexColors.Emplace(red_value * 1.5, 0.1, 0);
		}

		PMC->CreateMeshSection_LinearColor(0, verts, inds, TArray<FVector>(), TArray<FVector2D>(), VertexColors, TArray<FProcMeshTangent>(), false);
		PMC->SetMaterial(0, VertexMaterial);

	}


	//UE_LOG(LogTemp, Warning, TEXT("len verts = %d, len inds = %d"), verts.Num(), inds.Num());
	//draw function mesh
	
	//UE_LOG(LogTemp, Warning, TEXT("#color = %f %f %f"), VertexColors.Num());
	
	//UE_LOG(LogTemp, Warning, TEXT("is vis = %d"), PMC->IsMeshSectionVisible(0));
	//GetWorld()->LineBatcher->DrawMesh(verts, inds, FColor(0, 255, 0), SDPG_Foreground, 0.0f);
	GetWorld()->LineBatcher->DrawLine(FVector(-2000, -2000, 0), FVector(2000, -2000, 0), FLinearColor(1, 1, 1), SDPG_Foreground, 10.0f);
	GetWorld()->LineBatcher->DrawLine(FVector(-2000, -2000, 0), FVector(-2000, 2000, 0), FLinearColor(1, 1, 1), SDPG_Foreground, 10.0f);
	//draw simplex

	TArray<FVector> simplex_vertices;
	simplex_vertices.Emplace(Simplex[0].X * 400, Simplex[0].Y * 400, fn(Simplex[0].X, Simplex[0].Y));
	simplex_vertices.Emplace(Simplex[1].X * 400, Simplex[1].Y * 400, fn(Simplex[1].X, Simplex[1].Y));
	simplex_vertices.Emplace(Simplex[2].X * 400, Simplex[2].Y * 400, fn(Simplex[2].X, Simplex[2].Y));




	GetWorld()->LineBatcher->DrawLine(simplex_vertices[0], simplex_vertices[1], FLinearColor(0.5f, 0.0f, 0.5f), SDPG_Foreground, 10.0f);
	GetWorld()->LineBatcher->DrawLine(simplex_vertices[1], simplex_vertices[2], FLinearColor(0.5f, 0.0f, 0.5f), SDPG_Foreground, 10.0f);
	GetWorld()->LineBatcher->DrawLine(simplex_vertices[2], simplex_vertices[0], FLinearColor(0.5f, 0.0f, 0.5f), SDPG_Foreground, 10.0f);



	if (TimeSinceLastRedraw > 2) {
		TimeSinceLastRedraw = 0;

		//single simplex iteration
		auto lh = _min_max_ind(Simplex);
		auto l = lh.Get<0>();
		auto h = lh.Get<1>();
		auto centroid = _centroid(Simplex, h);

		UE_LOG(LogTemp, Warning, TEXT("X = %f, Y = %f"), centroid.X, centroid.Y);


		auto xr = _reflect(centroid, Simplex[h], Alpha);

		if (fn(xr.X, xr.Y) < fn(Simplex[l].X, Simplex[l].Y)) {
			auto xe = _expand(centroid, xr, Gamma);

			if (fn(xe.X, xe.Y) < fn(Simplex[l].X, Simplex[l].Y)) {
				Simplex[h] = xe;
			}
			else {
				Simplex[h] = xr;
			}
		}
		else {
			bool bContract = true;

			for (auto i = 0; i < Simplex.Num(); ++i) {
				if (i == h) continue;
				bContract &= (fn(xr.X, xr.Y) > fn(Simplex[i].X, Simplex[i].Y));
			}

			if (bContract) {
				if (fn(xr.X, xr.Y) < fn(Simplex[h].X, Simplex[h].Y)) {
					Simplex[h] = xr;
				}

				auto xk = _contract(centroid, Simplex[h], Beta);

				if (fn(xk.X, xk.Y) < fn(Simplex[h].X, Simplex[h].Y)) {
					Simplex[h] = xk;
				}
				else {
					for (auto i = 0; i < Simplex.Num(); ++i) {
						if (i == l) continue;

						Simplex[i] = 0.5 * (Simplex[l] + Simplex[i]);
					}
				}
			} else{
				Simplex[h] = xr;
			}

		}
		UE_LOG(LogTemp, Warning, TEXT("X = %f, Y = %f"), Simplex[0].X, Simplex[0].Y);

	}


}
/*
double AMyFunctionPlot::fn(double Valuex, double Valuey)
{
	
	//return Valuex * Valuex + Valuey * Valuey;
	return 100 * pow(Valuey - Valuex * Valuex, 2)
			+ pow((1 - Valuex), 2);
	
}*/


bool AMyFunctionPlot::ShouldTickIfViewportsOnly() const {
	return false;
}

void AMyFunctionPlot::BeginDestroy() {
	Super::BeginDestroy();

	//UE_LOG(LogTemp, Warning, TEXT("clearing PersistentLineBatcher"));
	//GetWorld()->PersistentLineBatcher->Flush();
}