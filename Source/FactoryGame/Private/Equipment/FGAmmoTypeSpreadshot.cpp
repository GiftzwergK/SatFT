// This file has been automatically generated by the Unreal Header Implementation tool

#include "Equipment/FGAmmoTypeSpreadshot.h"

UFGAmmoTypeSpreadshot::UFGAmmoTypeSpreadshot() : Super() {
	this->mNumShots = FInt32Interval(5, 10);
	this->mSpreadAngleDegrees = 0.0;
}
void UFGAmmoTypeSpreadshot::FireAmmunition_Implementation(){ }
void UFGAmmoTypeSpreadshot::ProcessHit_Implementation(const FHitResult& hitResult){ }
void UFGAmmoTypeSpreadshot::NetMulticast_PlayEffects_Implementation(const TArray< FHitResult >& traceResults){ }
void UFGAmmoTypeSpreadshot::PlayEffects_Implementation(const TArray< FHitResult >& traceResults){ }
