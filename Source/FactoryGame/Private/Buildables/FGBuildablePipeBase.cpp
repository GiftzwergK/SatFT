// This file has been automatically generated by the Unreal Header Implementation tool

#include "Buildables/FGBuildablePipeBase.h"
#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"
#include "Hologram/FGPipelineHologram.h"

AFGBuildablePipeBase::AFGBuildablePipeBase() : Super() {
	this->mMesh = nullptr;
	this->mMeshLength = 0.0;
	this->mConnection0 = nullptr;
	this->mConnection1 = nullptr;
	this->mSplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	this->mHologramClass = AFGPipelineHologram::StaticClass();
	this->NetDormancy = ENetDormancy::DORM_Awake;
	this->mSplineComponent->SetupAttachment(RootComponent);
}
void AFGBuildablePipeBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFGBuildablePipeBase, mSplineData);
	DOREPLIFETIME(AFGBuildablePipeBase, mSnappedPassthroughs);
}
void AFGBuildablePipeBase::BeginPlay(){ }
void AFGBuildablePipeBase::EndPlay(const EEndPlayReason::Type endPlayReason){ }
int32 AFGBuildablePipeBase::GetDismantleRefundReturnsMultiplier() const{ return int32(); }
bool AFGBuildablePipeBase::ShouldBeConsideredForBase_Implementation(){ return bool(); }
void AFGBuildablePipeBase::Upgrade_Implementation(AActor* newActor){ }
void AFGBuildablePipeBase::Dismantle_Implementation(){ }
void AFGBuildablePipeBase::GainedSignificance_Implementation(){ }
void AFGBuildablePipeBase::LostSignificance_Implementation(){ }
void AFGBuildablePipeBase::GainedSignificance_Native(){ }
void AFGBuildablePipeBase::LostSignificance_Native(){ }
void AFGBuildablePipeBase::SetupForSignificance(){ }
float AFGBuildablePipeBase::FindOffsetClosestToLocation(const FVector& location) const{ return float(); }
void AFGBuildablePipeBase::GetLocationAndDirectionAtOffset(float offset, FVector& out_location, FVector& out_direction) const{ }
TSubclassOf< class UFGPipeConnectionComponentBase > AFGBuildablePipeBase::GetConnectionType_Implementation(){ return TSubclassOf<class UFGPipeConnectionComponentBase>(); }
const float AFGBuildablePipeBase::PIPE_COST_LENGTH_MULTIPLIER = float();
