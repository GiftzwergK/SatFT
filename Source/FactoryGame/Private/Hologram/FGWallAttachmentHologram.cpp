// This file has been automatically generated by the Unreal Header Implementation tool

#include "Hologram/FGWallAttachmentHologram.h"

AFGWallAttachmentHologram::AFGWallAttachmentHologram() : Super() {
	this->mCanSnapToFoundationFloor = false;
	this->mCanSnapToWalls = true;
	this->mSnapAxis = EAxis::X;
}
bool AFGWallAttachmentHologram::TrySnapToActor(const FHitResult& hitResult){ return bool(); }
void AFGWallAttachmentHologram::CheckValidFloor(){ }
