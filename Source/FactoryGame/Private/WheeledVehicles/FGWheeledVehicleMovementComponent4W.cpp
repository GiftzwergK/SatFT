// This file has been automatically generated by the Unreal Header Implementation tool

#include "WheeledVehicles/FGWheeledVehicleMovementComponent4W.h"
#include "VehicleWheel.h"

UFGWheeledVehicleMovementComponent4W::UFGWheeledVehicleMovementComponent4W(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	this->mUseDSOLGearbox = true;
	this->mDSOLMaxSlopeAngleLimit = 33.0;
	this->mSlopeShiftRatio = 0.3;
	this->mDownShiftLatency = 2.0;
	this->WheelSetups.Emplace();
	this->WheelSetups[0].WheelClass = UVehicleWheel::StaticClass();
	this->WheelSetups[0].BoneName = TEXT("None");
	this->WheelSetups[0].AdditionalOffset = FVector::ZeroVector;
	this->WheelSetups[0].bDisableSteering = false;
	this->WheelSetups.Emplace();
	this->WheelSetups[1].WheelClass = UVehicleWheel::StaticClass();
	this->WheelSetups[1].BoneName = TEXT("None");
	this->WheelSetups[1].AdditionalOffset = FVector::ZeroVector;
	this->WheelSetups[1].bDisableSteering = false;
	this->WheelSetups.Emplace();
	this->WheelSetups[2].WheelClass = UVehicleWheel::StaticClass();
	this->WheelSetups[2].BoneName = TEXT("None");
	this->WheelSetups[2].AdditionalOffset = FVector::ZeroVector;
	this->WheelSetups[2].bDisableSteering = false;
	this->WheelSetups.Emplace();
	this->WheelSetups[3].WheelClass = UVehicleWheel::StaticClass();
	this->WheelSetups[3].BoneName = TEXT("None");
	this->WheelSetups[3].AdditionalOffset = FVector::ZeroVector;
	this->WheelSetups[3].bDisableSteering = false;
}
bool UFGWheeledVehicleMovementComponent4W::GetHandbreakInput() const{ return bool(); }
void UFGWheeledVehicleMovementComponent4W::UpdateSimulation(float DeltaTime){ }
void UFGWheeledVehicleMovementComponent4W::GenerateTireForces( UVehicleWheel* Wheel, const FTireShaderInput& Input, FTireShaderOutput& Output){ }
void UFGWheeledVehicleMovementComponent4W::SetupWheelMassProperties_AssumesLocked(const uint32 NumWheels, physx::PxVehicleWheelsSimData* PWheelsSimData, physx::PxRigidBody* PVehicleActor){ }
void UFGWheeledVehicleMovementComponent4W::SetupWheels(physx::PxVehicleWheelsSimData* PWheelsSimData){ }
void UFGWheeledVehicleMovementComponent4W::PreTick(float DeltaTime){ }
void UFGWheeledVehicleMovementComponent4W::SetupVehicleShapes(){ }
float UFGWheeledVehicleMovementComponent4W::GetLargestTireLoadValue(){ return float(); }
float UFGWheeledVehicleMovementComponent4W::GetLargestLateralSlip(){ return float(); }
float UFGWheeledVehicleMovementComponent4W::GetLargestLongitudinalSlip(){ return float(); }
