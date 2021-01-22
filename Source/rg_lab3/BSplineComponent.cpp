// Fill out your copyright notice in the Description page of Project Settings.
#include "BSplineComponent.h"





// Sets default values for this component's properties
UBSplineComponent::UBSplineComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	std::string spline_path("C:\\Users\\borna\\Documents\\Unreal Projects\\rg_lab1\\Intermediate\\ProjectFiles\\");
	spline_path += "bspline_sphere.txt";
	std::ifstream stream_control_pts(spline_path);


	float x, y, z;
	while (stream_control_pts >> x >> y >> z) {
		control_points.Emplace(x, y, z);
		UE_LOG(LogTemp, Warning, TEXT("Loading control points %f %f %f"), x, y, z);
	}

    UE_LOG(LogTemp, Warning, TEXT("A\nB\nC\nD"), x, y, z);

}


// Called when the game starts
void UBSplineComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBSplineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    static float elapsed_animation_time = 0;                            //for how long has the current iteration of animation been running for
    float total_animation_time = 100;                                    //in seconds, how long should the animation last
    float scale = (control_points.Num() - 3)/ total_animation_time;     //linear mapping between current time and desired animation parameter (animation parameter goes 0 - num_segments)
    float parameter = elapsed_animation_time * scale;                   //current animation parameter e.g. 1.25 is first segment, t = 0.25 
    int which_segment = (int)parameter;
    float param_in_segment = parameter - which_segment;

    if (AActor* owner = GetOwner())
    {
        //UE_LOG(LogTemp, Warning, TEXT("SetActorLocation"));
        
        

        //Draw tangent line
        FVector point_on_curve = segment_for_param(param_in_segment, 
            control_points[which_segment], control_points[which_segment + 1], control_points[which_segment + 2], control_points[which_segment + 3]);

        FVector tangent_vector = segment_tangent_for_param(param_in_segment, 
            control_points[which_segment], control_points[which_segment + 1], control_points[which_segment + 2], control_points[which_segment + 3]);

        FVector dd_curve_vector = segment_dd_for_param(param_in_segment,
            control_points[which_segment], control_points[which_segment + 1], control_points[which_segment + 2], control_points[which_segment + 3]);

        //define target coordinate system for orientation
        FVector axis_u = FVector::CrossProduct(tangent_vector, dd_curve_vector);
        axis_u.Normalize();
        FVector axis_w = tangent_vector;
        axis_w.Normalize();
        FVector axis_v = FVector::CrossProduct(axis_w, axis_u);
        axis_v.Normalize();

        FMatrix rotM = FRotationMatrix::MakeFromXZ(-axis_v, axis_w);
        
        if (!owner->SetActorLocation(FVector(0, 0, 0))) {
            UE_LOG(LogTemp, Warning, TEXT("could not move"));
        }

        if (!owner->SetActorRotation(FQuat(rotM))) {
            UE_LOG(LogTemp, Warning, TEXT("could not rotate"));
        }

        
        //move owner mesh
        if (!owner->SetActorLocation(point_on_curve)) {
            UE_LOG(LogTemp, Warning, TEXT("could not move"));
        }


        //draw debug point
        GetWorld()->LineBatcher->DrawPoint(point_on_curve, FLinearColor(0.7f, 0.4f, 0.1f), 7.f, SDPG_Foreground);

        //draw tangent line
        GetWorld()->LineBatcher->DrawLine(point_on_curve, point_on_curve + 5 * axis_u, FLinearColor(1.f, 0.f, 0.f), .5f, SDPG_Foreground);
        GetWorld()->LineBatcher->DrawLine(point_on_curve, point_on_curve + 5 * axis_w, FLinearColor(0.f, 1.f, 0.f), .5f, SDPG_Foreground);
        GetWorld()->LineBatcher->DrawLine(point_on_curve, point_on_curve + 5 * axis_v, FLinearColor(0.f, 0.f, 1.f), .5f, SDPG_Foreground);


    }
    
    //Draw control points
    for (auto i = 0; i < control_points.Num(); ++i) {
        GetWorld()->LineBatcher->DrawPoint(FVector(control_points[i]),
            FLinearColor(1.f, 1.f, 1.f), 5.0f, SDPG_Foreground);
    }

    for (auto seg_i = 1; seg_i <= control_points.Num() - 3; ++seg_i) {
        for (auto i = 0u; i < 301; ++i) {
            FVector point_on_curve = segment_for_param(
                i / (float)301, control_points[seg_i - 1], control_points[seg_i], control_points[seg_i + 1], control_points[seg_i + 2]);

            //GetWorld()->LineBatcher->DrawLine(point_on_curve, line_end, FLinearColor(0.5f, 0.f, 0.f), 1.f, SDPG_Foreground);
            GetWorld()->LineBatcher->DrawPoint(point_on_curve, FLinearColor(1.f, 1.f, 0.f), 3.f, SDPG_Foreground);
                //FLinearColor(0.5f, 0.f, 1.f), 2.0f, SDPG_Foreground);
        }
    }

    elapsed_animation_time += DeltaTime;
    if (elapsed_animation_time > total_animation_time) {
        elapsed_animation_time = 0; //reset animation
    }
}


FVector UBSplineComponent::segment_for_param(float t, FVector r1, FVector r2, FVector r3, FVector r4) {
    float t2 = pow(t, 2.f);
    float t3 = pow(t, 3.f);

    float pol1 = (-t3 + 3 * t2 - 3 * t + 1) / 6.0f;
    float pol2 = (3 * t3 - 6 * t2 + 4) / 6.0f;
    float pol3 = (-3 * t3 + 3 * t2 + 3 * t + 1) / 6.0f;
    float pol4 = (t3) / 6.0f;


    return FVector(
        r1.X * pol1 + r2.X * pol2 + r3.X * pol3 + r4.X * pol4,
        r1.Y * pol1 + r2.Y * pol2 + r3.Y * pol3 + r4.Y * pol4,
        r1.Z * pol1 + r2.Z * pol2 + r3.Z * pol3 + r4.Z * pol4
    );
}

FVector UBSplineComponent::segment_tangent_for_param(float t, FVector r1, FVector r2, FVector r3, FVector r4) {
    float t2 = pow(t, 2.f);

    float pol1 = (-t2 + 2*t - 1) / 2.0f;
    float pol2 = (3*t2 - 4 *t) / 2.0f;
    float pol3 = (-3*t2 + 2*t + 1) / 2.0f;
    float pol4 = (t2) / 2.0f;


    return FVector(
        r1.X * pol1 + r2.X * pol2 + r3.X * pol3 + r4.X * pol4,
        r1.Y * pol1 + r2.Y * pol2 + r3.Y * pol3 + r4.Y * pol4,
        r1.Z * pol1 + r2.Z * pol2 + r3.Z * pol3 + r4.Z * pol4
    );
}

//second derivative
FVector UBSplineComponent::segment_dd_for_param(float t, FVector r1, FVector r2, FVector r3, FVector r4) {

    float pol1 = -t + 1;
    float pol2 = 3 * t - 2;
    float pol3 = -3 * t + 1;
    float pol4 = t;


    return FVector(
        r1.X * pol1 + r2.X * pol2 + r3.X * pol3 + r4.X * pol4,
        r1.Y * pol1 + r2.Y * pol2 + r3.Y * pol3 + r4.Y * pol4,
        r1.Z * pol1 + r2.Z * pol2 + r3.Z * pol3 + r4.Z * pol4
    );
}