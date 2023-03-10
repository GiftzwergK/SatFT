// This file has been automatically generated by the Unreal Header Implementation tool

#include "FGBlueprintProxy.h"
#include "Components/SceneComponent.h"

#if !UE_BUILD_SHIPPING
void AFGBlueprintProxy::DisplayDebugInfo() const{ }
#endif 
AFGBlueprintProxy::AFGBlueprintProxy() : Super() {
	this->mBlueprintName = INVTEXT("");
	this->mLocalBounds = FBox(FVector::ZeroVector, FVector::ZeroVector);
	this->mBlueprintDescriptor = nullptr;
	this->mBoundingBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Bounding Box"));
	this->bReplicates = true;
	this->RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	this->mBoundingBox->SetupAttachment(RootComponent);
}
void AFGBlueprintProxy::BeginPlay(){ }
void AFGBlueprintProxy::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFGBlueprintProxy, mBlueprintName);
	DOREPLIFETIME(AFGBlueprintProxy, mLocalBounds);
	DOREPLIFETIME(AFGBlueprintProxy, mBuildables);
}
void AFGBlueprintProxy::Tick(float DeltaSeconds){ }
void AFGBlueprintProxy::RegisterBuildable( AFGBuildable* buildable){ }
void AFGBlueprintProxy::UnregisterBuildable( AFGBuildable* buildable){ }
int32 AFGBlueprintProxy::CollectBuildables(TArray<  AFGBuildable* >& out_buildables) const{ return int32(); }
void AFGBlueprintProxy::PreSaveGame_Implementation(int32 saveVersion, int32 gameVersion){ }
void AFGBlueprintProxy::PostSaveGame_Implementation(int32 saveVersion, int32 gameVersion){ }
void AFGBlueprintProxy::PreLoadGame_Implementation(int32 saveVersion, int32 gameVersion){ }
void AFGBlueprintProxy::PostLoadGame_Implementation(int32 saveVersion, int32 gameVersion){ }
void AFGBlueprintProxy::GatherDependencies_Implementation(TArray< UObject* >& out_dependentObjects){ }
bool AFGBlueprintProxy::NeedTransform_Implementation(){ return bool(); }
bool AFGBlueprintProxy::ShouldSave_Implementation() const{ return bool(); }
void AFGBlueprintProxy::OnRep_BlueprintName(){ }
void AFGBlueprintProxy::OnRep_LocalBounds(){ }
void AFGBlueprintProxy::AssignBuildables(const TArray<  AFGBuildable* >& buildables){ }
