// Copyright 2023 metabotics.ai

#include "SkidDriveComponent.h"

// rclUE
#include "rclcUtilities.h"

// RapyutaSimulationPlugins
#include "Core/RRConversionUtils.h"

void USkidDriveComponent::SetWheels(
  UPhysicsConstraintComponent* InWheelFrontLeft,
  UPhysicsConstraintComponent* InWheelFrontRight,
  UPhysicsConstraintComponent* InWheelRearLeft,
  UPhysicsConstraintComponent* InWheelRearRight)
{
    auto fSetWheel = [this](UPhysicsConstraintComponent*& CurWheel, UPhysicsConstraintComponent* NewWheel)
    {
        if (IsValid(NewWheel))
        {
            CurWheel = NewWheel;
            CurWheel->SetAngularDriveMode(EAngularDriveMode::TwistAndSwing);
            CurWheel->SetAngularDriveParams(MaxForce, MaxForce, MaxForce);
            CurWheel->SetAngularVelocityDriveTwistAndSwing(true, false);
        }
        else
        {
            //UE_LOG_WITH_INFO_NAMED(LogDifferentialDriveComponent, Error, TEXT("NewWheel is invalid!"));
            UE_LOG(LogTemp, Error, TEXT("NewWheel is invalid!"));
        }
    };

    fSetWheel(WheelFrontLeft, InWheelFrontLeft);
    fSetWheel(WheelFrontRight, InWheelFrontRight);
    fSetWheel(WheelRearLeft, InWheelRearLeft);
    fSetWheel(WheelRearRight, InWheelRearRight);
}

void USkidDriveComponent::SetPerimeter()
{
    if (WheelRadius <= 1e-6)
    {
        WheelRadius = 1.0f;
        //UE_LOG_WITH_INFO_NAMED(
        //    LogDifferentialDriveComponent, Warning, TEXT("Wheel radius is too small. Wheel radius is reset to 1.0"));
        UE_LOG(
            LogTemp, Warning, TEXT("Wheel radius is too small. Wheel radius is reset to 1.0"));
    }
    WheelPerimeter = WheelRadius * 2.f * M_PI;
}

void USkidDriveComponent::TickComponent(float InDeltaTime,
                                                enum ELevelTick TickType,
                                                FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(InDeltaTime, TickType, ThisTickFunction);
    if (!ShouldSkipUpdate(InDeltaTime))
    {
        UpdateOdom(InDeltaTime);
    }
}

void USkidDriveComponent::UpdateMovement(float DeltaTime)
{
    if (IsValid(WheelFrontLeft) && IsValid(WheelFrontRight) && IsValid(WheelRearLeft) && IsValid(WheelRearRight))
    {
        float velL = Velocity.X + AngularVelocity.Z * WheelSeparationHalf * WheelSeparationMultiplier;
        float velR = Velocity.X - AngularVelocity.Z * WheelSeparationHalf * WheelSeparationMultiplier;

        WheelFrontLeft->SetAngularVelocityTarget(FVector(velL / WheelPerimeter, 0, 0));
        WheelFrontRight->SetAngularVelocityTarget(FVector(velR / WheelPerimeter, 0, 0));
        WheelRearLeft->SetAngularVelocityTarget(FVector(velL / WheelPerimeter, 0, 0));
        WheelRearRight->SetAngularVelocityTarget(FVector(velR / WheelPerimeter, 0, 0));
        WheelFrontLeft->SetAngularDriveParams(MaxForce, MaxForce, MaxForce);
        WheelFrontRight->SetAngularDriveParams(MaxForce, MaxForce, MaxForce);
        WheelRearLeft->SetAngularDriveParams(MaxForce, MaxForce, MaxForce);
        WheelRearRight->SetAngularDriveParams(MaxForce, MaxForce, MaxForce);
    }
    else
    {
        //UE_LOG_WITH_INFO_NAMED(LogDifferentialDriveComponent, Error, TEXT("Wheel Joints are not set"));
        UE_LOG(LogTemp, Error, TEXT("Wheel Joints are not set"));
    }
}

void USkidDriveComponent::UpdateOdom(float DeltaTime)
{
    if (OdomComponent == nullptr)
    {
        return;
    }

    if (!OdomComponent->bIsOdomInitialized)
    {
        OdomComponent->InitOdom();
        PoseEncoderX = 0;
        PoseEncoderY = 0;
        PoseEncoderTheta = 0;
    }

    FROSOdom odomData = OdomComponent->OdomData;

    // time
    odomData.Header.Stamp = URRConversionUtils::FloatToROSStamp(UGameplayStatics::GetTimeSeconds(GetWorld()));

    // vl and vr as computed here is ok for kinematics
    // for physics, vl and vr should be computed based on the change in wheel orientation (i.e. the velocity term to be used is
    // wheel rotations per unit time [rad/s]) together with the wheel radius or perimeter, the displacement can be computed:
    //  vl = (left_wheel_orientation_rad_now - left_wheel_orientation_rad_previous) * perimeter / (2pi)
    //  vr = (right_wheel_orientation_rad_now - right_wheel_orientation_rad_previous) * perimeter / (2pi)
    // in the kinematics case, (dx,dy,dtheta) can be simplified considerably
    // but as this is not a performance bottleneck, for the moment we leave the full general formulation,
    // at least until the odom for the physics version of the agent is implemented, so that we have a reference
    float vl = Velocity.X + AngularVelocity.Z * WheelSeparationHalf * WheelSeparationMultiplier;
    float vr = Velocity.X - AngularVelocity.Z * WheelSeparationHalf * WheelSeparationMultiplier;

    // noise added as a component of vl, vr
    // Gazebo links this Book here: Sigwart 2011 Autonomous Mobile Robots page:337
    //  seems to be Introduction to Autonomous Mobile Robots (Sigwart, Nourbakhsh, Scaramuzza)
    float sl = (vl + OdomComponent->bWithNoise * OdomComponent->GaussianRNGPosition(OdomComponent->Gen)) * DeltaTime;
    float sr = (vr + OdomComponent->bWithNoise * OdomComponent->GaussianRNGPosition(OdomComponent->Gen)) * DeltaTime;
    float ssum = sl + sr;

    float sdiff = sr - sl;

    float dx = ssum * .5f * cos(PoseEncoderTheta + sdiff / (4.f * WheelSeparationHalf * WheelSeparationMultiplier));
    float dy = ssum * .5f * sin(PoseEncoderTheta + sdiff / (4.f * WheelSeparationHalf * WheelSeparationMultiplier));
    float dtheta = -sdiff / (2.f * WheelSeparationHalf * WheelSeparationMultiplier);

    PoseEncoderX += dx;
    PoseEncoderY += dy;
    PoseEncoderTheta += dtheta;

    float w = dtheta / DeltaTime;
    float v = sqrt(dx * dx + dy * dy) / DeltaTime;

    // FRotator is in degrees, while PoseEncoderTheta is in Radians
    FQuat qt(FRotator(0, FMath::RadiansToDegrees(PoseEncoderTheta), 0));

    odomData.Pose.Pose.Position.X = PoseEncoderX;
    odomData.Pose.Pose.Position.Y = PoseEncoderY;
    odomData.Pose.Pose.Position.Z = 0;

    odomData.Pose.Pose.Orientation = qt;

    odomData.Twist.Twist.Angular.Z = w;
    odomData.Twist.Twist.Linear.X = v;
    odomData.Twist.Twist.Linear.Y = 0;
    odomData.Twist.Twist.Linear.Z = 0;

    odomData.Pose.Covariance[0] = 0.01;
    odomData.Pose.Covariance[7] = 0.01;
    odomData.Pose.Covariance[14] = 1e+12;
    odomData.Pose.Covariance[21] = 1e+12;
    odomData.Pose.Covariance[28] = 1e+12;
    odomData.Pose.Covariance[35] = 0.01;
    odomData.Twist.Covariance[0] = 0.01;
    odomData.Twist.Covariance[7] = 0.01;
    odomData.Twist.Covariance[14] = 1e+12;
    odomData.Twist.Covariance[21] = 1e+12;
    odomData.Twist.Covariance[28] = 1e+12;
    odomData.Twist.Covariance[35] = 0.01;

    OdomComponent->OdomData = odomData;

    // UE_LOG_WITH_INFO(LogTemp, Warning, TEXT("Input:"));
    // UE_LOG_WITH_INFO(LogTemp, Warning, TEXT("\tVel: %s, %s"), *Velocity.ToString(), *AngularVelocity.ToString());
    // UE_LOG_WITH_INFO(LogTemp, Warning, TEXT("Odometry:"));
    // UE_LOG_WITH_INFO(LogTemp, Warning, TEXT("\tOdom Positon:\t\t\t\t%f %f from %f %f (%f)"), PoseEncoderX, PoseEncoderY, dx, dy,
    // Velocity.X); UE_LOG_WITH_INFO(LogTemp, Warning, TEXT("\tOdom Orientation:\t\t\t%s (%f)"), *OdomData.Pose.Pose.Orientation.ToString(),
    // PoseEncoderTheta); UE_LOG_WITH_INFO(LogTemp, Warning, TEXT("\tOdom TwistLin:\t\t\t\t%s - %f"), *OdomData.Twist.Twist.Linear.ToString(),
    // OdomData.Twist.Twist.Linear.Size()); UE_LOG_WITH_INFO(LogTemp, Warning, TEXT("\tOdom TwistAng:\t\t\t\t%s"),
    // *OdomData.Twist.Twist.Angular.ToString());
}

void USkidDriveComponent::Initialize()
{
    Super::Initialize();
    SetPerimeter();
    if (OdomComponent)
    {
        // Odom update is done by this class instead of OdomComponent.
        OdomComponent->bManualUpdate = true;
    }
}
