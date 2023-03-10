// This file has been automatically generated by the Unreal Header Implementation tool

#include "FGPriorityPowerSwitchInfo.h"

AFGPriorityPowerSwitchInfo::AFGPriorityPowerSwitchInfo() : Super() {
	this->mSwitchName = TEXT("");
	this->mIsSwitchOn = false;
	this->bAlwaysRelevant = true;
	this->bReplicates = true;
}
void AFGPriorityPowerSwitchInfo::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFGPriorityPowerSwitchInfo, mSwitchName);
	DOREPLIFETIME(AFGPriorityPowerSwitchInfo, mSwitchPriority);
	DOREPLIFETIME(AFGPriorityPowerSwitchInfo, mIsSwitchOn);
}
void AFGPriorityPowerSwitchInfo::BeginPlay(){ }
void AFGPriorityPowerSwitchInfo::EndPlay(const EEndPlayReason::Type endPlayReason){ }
void AFGPriorityPowerSwitchInfo::Init( AFGBuildablePriorityPowerSwitch* prioritySwitch){ }
void AFGPriorityPowerSwitchInfo::SetPriority(int32 priority){ }
void AFGPriorityPowerSwitchInfo::SetSwitchOn(bool on){ }
void AFGPriorityPowerSwitchInfo::OnSwitchBuildingTagChanged(bool hasTag, const FString& tag){ }
void AFGPriorityPowerSwitchInfo::OnIsSwitchOnChanged(){ }
void AFGPriorityPowerSwitchInfo::OnSwitchPriorityChanged(int32 priority){ }
