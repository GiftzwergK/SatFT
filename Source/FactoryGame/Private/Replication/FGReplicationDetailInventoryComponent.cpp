// This file has been automatically generated by the Unreal Header Implementation tool

#include "Replication/FGReplicationDetailInventoryComponent.h"
#include "FGInventoryComponent.h"

UFGReplicationDetailInventoryComponent::UFGReplicationDetailInventoryComponent() : Super() {
	this->mActiveInventoryComponent = nullptr;
	this->mMainInventoryComponent = nullptr;
	this->mReplicationInventoryComponent = nullptr;
	this->mReplicationDetailActor = nullptr;
}
void UFGReplicationDetailInventoryComponent::SetMainInventoryComponent( UFGInventoryComponent* inventoryComponent){ }
void UFGReplicationDetailInventoryComponent::SetReplicationInventoryComponent( UFGInventoryComponent* inventoryComponent){ }
void UFGReplicationDetailInventoryComponent::ClearReplicationInventoryComponent(){ }
