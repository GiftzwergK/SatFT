// This file has been automatically generated by the Unreal Header Implementation tool

#include "FGOutlineComponent.h"
#include "Components/StaticMeshComponent.h"

UFGOutlineComponent::UFGOutlineComponent() : Super() {
	this->mOutlineProxy = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OutlineProxy"));
	this->mOutlineProxy->SetupAttachment(this);
	this->mActiveOutlineActor = nullptr;
}
UFGOutlineComponent* UFGOutlineComponent::Get(UWorld* world){ return nullptr; }
void UFGOutlineComponent::ShowOutline(AActor* actorToOutline, EOutlineColor color){ }
void UFGOutlineComponent::ShowProxyOutline( UStaticMesh* outlineMesh, const FTransform transform, EOutlineColor color){ }
void UFGOutlineComponent::ShowMultiActorOutline(TArray< AActor* > actorsToOutline, EOutlineColor color){ }
void UFGOutlineComponent::UpdateProxyOutlineMesh( UStaticMesh* newOutlineMesh){ }
void UFGOutlineComponent::UpdateProxyOutlineLocationAndRotation(FVector newLocation, FRotator newRotation){ }
void UFGOutlineComponent::HideOutline(){ }
void UFGOutlineComponent::ShowActorOutline( AActor* actorToOutline, EOutlineColor color){ }
void UFGOutlineComponent::HideActorOutline( AActor* actorToDisableOutlineOn){ }
void UFGOutlineComponent::ShowDismantlePendingMaterial(TArray< AActor* > actorPendingDismantle){ }
void UFGOutlineComponent::ShowDismantlePendingMaterial(AActor* actorPendingDismantle){ }
void UFGOutlineComponent::ShowDismantlePendingMaterialOnActor(AActor* actorPendingForDismantle){ }
bool UFGOutlineComponent::IsOwnedByLocalPlayer(){ return bool(); }
