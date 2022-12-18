#include "FGGameState.h"

#include "FGBuildableSubsystem.h"
#include "FGGameSession.h"
#include "FGPlayerController.h"
#include "FGStorySubsystem.h"
#include "FGTutorialIntroManager.h"
#include "Buildables/FGBuildable.h"

AFGGameState::AFGGameState() : Super() {
	this->mTurboModeMultiplier = 0.0;
	this->mPowerCircuitFuseTriggeredMessage = nullptr;
	this->mTimeSubsystem = nullptr;
	this->mStorySubsystem = nullptr;
	this->mRailroadSubsystem = nullptr;
	this->mCircuitSubsystem = nullptr;
	this->mRecipeManager = nullptr;
	this->mSchematicManager = nullptr;
	this->mGamePhaseManager = nullptr;
	this->mResearchManager = nullptr;
	this->mTutorialIntroManager = nullptr;
	this->mActorRepresentationManager = nullptr;
	this->mMapManager = nullptr;
	this->mRadioactivitySubsystem = nullptr;
	this->mChatManager = nullptr;
	this->mPipeSubsystem = nullptr;
	this->mUnlockSubsystem = nullptr;
	this->mResourceSinkSubsystem = nullptr;
	this->mItemRegrowSubsystem = nullptr;
	this->mVehicleSubsystem = nullptr;
	this->mEventSubsystem = nullptr;
	this->mWorldGridSubsystem = nullptr;
	this->mDroneSubsystem = nullptr;
	this->mStatisticsSubsystem = nullptr;
	this->mSignSubsystem = nullptr;
	this->mCreatureSubsystem = nullptr;
	this->mScannableSubsystem = nullptr;
	this->mBlueprintSubsystem = nullptr;
	this->mReplicatedSessionName = TEXT("");
	this->mUnlockCustomizerSchematic = nullptr;
	this->mPlannedRestartTime = 24.0;
	this->mHubPartClass = nullptr;
	this->mForceAddHubPartOnSpawn = false;
	this->mCheatNoCost = false;
	this->mCheatNoPower = false;
	this->mCheatNoFuel = false;
	this->mIsTradingPostBuilt = false;
	this->mHasInitalTradingPostLandAnimPlayed = false;
	this->mIsSpaceElevatorBuilt = false;
	this->mPublicTodoList = TEXT("");
	this->PrimaryActorTick.TickGroup = ETickingGroup::TG_PrePhysics;
	this->PrimaryActorTick.EndTickGroup = ETickingGroup::TG_PrePhysics;
	this->PrimaryActorTick.bTickEvenWhenPaused = true;
	this->PrimaryActorTick.bCanEverTick = true;
	this->PrimaryActorTick.bStartWithTickEnabled = true;
	this->PrimaryActorTick.bAllowTickOnDedicatedServer = true;
	this->PrimaryActorTick.TickInterval = 5.0;
}

void AFGGameState::Serialize(FArchive& ar) {
	Super::Serialize(ar);
	if (ar.IsSaveGame()) {
		ar << PlayerArray;
	}
}

void AFGGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFGGameState, mTimeSubsystem);
	DOREPLIFETIME(AFGGameState, mStorySubsystem);
	DOREPLIFETIME(AFGGameState, mRailroadSubsystem);
	DOREPLIFETIME(AFGGameState, mCircuitSubsystem);
	DOREPLIFETIME(AFGGameState, mRecipeManager);
	DOREPLIFETIME(AFGGameState, mSchematicManager);
	DOREPLIFETIME(AFGGameState, mGamePhaseManager);
	DOREPLIFETIME(AFGGameState, mResearchManager);
	DOREPLIFETIME(AFGGameState, mTutorialIntroManager);
	DOREPLIFETIME(AFGGameState, mActorRepresentationManager);
	DOREPLIFETIME(AFGGameState, mMapManager);
	DOREPLIFETIME(AFGGameState, mChatManager);
	DOREPLIFETIME(AFGGameState, mPipeSubsystem);
	DOREPLIFETIME(AFGGameState, mUnlockSubsystem);
	DOREPLIFETIME(AFGGameState, mResourceSinkSubsystem);
	DOREPLIFETIME(AFGGameState, mVehicleSubsystem);
	DOREPLIFETIME(AFGGameState, mEventSubsystem);
	DOREPLIFETIME(AFGGameState, mDroneSubsystem);
	DOREPLIFETIME(AFGGameState, mSignSubsystem);
	DOREPLIFETIME(AFGGameState, mCreatureSubsystem);
	DOREPLIFETIME(AFGGameState, mScannableSubsystem);
	DOREPLIFETIME(AFGGameState, mBlueprintSubsystem);
	DOREPLIFETIME(AFGGameState, mVisitedMapAreas);
	DOREPLIFETIME(AFGGameState, mPickedUpItems);
	DOREPLIFETIME(AFGGameState, mPlayDurationWhenLoaded);
	DOREPLIFETIME(AFGGameState, mReplicatedSessionName);
	DOREPLIFETIME(AFGGameState, mReplicadedOnlineNumPubliclConnections);
	DOREPLIFETIME(AFGGameState, mBuildingColorSlots_Data);
	DOREPLIFETIME(AFGGameState, mBuildableLightColorSlots);
	DOREPLIFETIME(AFGGameState, mPlayerGlobalColorPresets);
	DOREPLIFETIME(AFGGameState, mSwatchGroupDatum);
	DOREPLIFETIME(AFGGameState, mPlannedRestartTime);
	DOREPLIFETIME(AFGGameState, mCheatNoCost);
	DOREPLIFETIME(AFGGameState, mCheatNoPower);
	DOREPLIFETIME(AFGGameState, mCheatNoFuel);
	DOREPLIFETIME(AFGGameState, mIsTradingPostBuilt);
	DOREPLIFETIME(AFGGameState, mHasInitalTradingPostLandAnimPlayed);
	DOREPLIFETIME(AFGGameState, mIsSpaceElevatorBuilt);
	DOREPLIFETIME(AFGGameState, mServerLocalDateTimeTicksAtInit);
	DOREPLIFETIME(AFGGameState, mTetrominoLeaderBoard);
	DOREPLIFETIME(AFGGameState, mPublicTodoList);
}

void AFGGameState::Tick(float delta) {
	Super::Tick(delta);
	CheckRestartTime();

	if (const AGameModeBase* GameMode = GetWorld()->GetAuthGameMode()) {
		int32 MaxPlayers = 0;
		if (const AFGGameSession* GameSession = Cast<AFGGameSession>(GameMode->GameSession)) {
			MaxPlayers = GameSession->GetNumPublicConnections();
		}
		if (MaxPlayers != mReplicadedOnlineNumPubliclConnections) {
			mReplicadedOnlineNumPubliclConnections = MaxPlayers;
		}
	}
	FString& Wtf = FString();
}

void AFGGameState::BeginPlay() {
	Super::BeginPlay();
	if (!HasAuthority()) {
		CheckClientSubsystemsValid();
	}
	SubmitNumPlayersTelemetry();
}

void AFGGameState::PreSaveGame_Implementation(int32 saveVersion, int32 gameVersion) {
	mPlayDurationWhenLoaded += (int32)GetServerWorldTimeSeconds();
}

void AFGGameState::PostSaveGame_Implementation(int32 saveVersion, int32 gameVersion) {
	mPlayDurationWhenLoaded -= (int32)GetServerWorldTimeSeconds();
}

void AFGGameState::PreLoadGame_Implementation(int32 saveVersion, int32 gameVersion) {
}

void AFGGameState::PostLoadGame_Implementation(int32 saveVersion, int32 gameVersion) {
}

void AFGGameState::GatherDependencies_Implementation(TArray< UObject* >& out_dependentObjects) {
}

bool AFGGameState::NeedTransform_Implementation() {
	return false;
}

bool AFGGameState::ShouldSave_Implementation() const {
	return true;
}

void AFGGameState::HandleMatchIsWaitingToStart() {
	Super::HandleMatchIsWaitingToStart();
}

void AFGGameState::HandleMatchHasStarted() {
	GEngine->BlockTillLevelStreamingCompleted(GetWorld());
	Super::HandleMatchHasStarted();
}

void AFGGameState::AddPlayerState(APlayerState* playerState) {
	Super::AddPlayerState(playerState);
	
	if (AFGPlayerState* PlayerState = Cast<AFGPlayerState>(playerState)) {
		if (!PlayerState->IsInactive() && HasAuthority()) {
			PlayerState->SetSlotNum(FindFreeSlot(PlayerState));
		}
		PlayerState->mOnColorDataUpdated.AddDynamic(this, &AFGGameState::OnPlayerStateSlotDataUpdated);
	}
	if (HasBegunPlay()) {
		SubmitNumPlayersTelemetry();
	}
}

void AFGGameState::RemovePlayerState(APlayerState* playerState) {
	if (AFGPlayerState* PlayerState = Cast<AFGPlayerState>(playerState)) {
		PlayerState->mOnColorDataUpdated.RemoveDynamic(this, &AFGGameState::OnPlayerStateSlotDataUpdated);
	}
	Super::RemovePlayerState(playerState);
	if (HasBegunPlay()) {
		SubmitNumPlayersTelemetry();
	}
}

void AFGGameState::OnPlayerStateSlotDataUpdated(AFGPlayerState* playerState) {
	this->mOnPlayerStateSlotDataUpdated.Broadcast(playerState);
}

void AFGGameState::Init() {
	
}

bool AFGGameState::AreClientSubsystemsValid() {
	return this->mTimeSubsystem &&
		this->mStorySubsystem &&
	 	this->mRailroadSubsystem &&
	 	this->mCircuitSubsystem &&
	 	this->mRecipeManager &&
	 	this->mSchematicManager &&
	 	this->mGamePhaseManager &&
	 	this->mResearchManager &&
	 	this->mTutorialIntroManager &&
	 	this->mPipeSubsystem &&
	 	this->mActorRepresentationManager &&
	 	this->mMapManager &&
	 	this->mChatManager &&
	 	this->mResourceSinkSubsystem &&
	 	this->mVehicleSubsystem &&
	 	this->mEventSubsystem &&
	 	this->mDroneSubsystem &&
	 	this->mSignSubsystem &&
	 	this->mScannableSubsystem &&
	 	this->mCreatureSubsystem &&
	 	this->mBlueprintSubsystem;
}

void AFGGameState::CheckClientSubsystemsValid() {
	if (!HasAuthority()) {
		if (AreClientSubsystemsValid()) {
			mOnClientSubsystemsValid.Broadcast();
			return;
		}
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, CheckClientSubsystemsValid);
	}
}

int32 AFGGameState::FindFreeSlot(AFGPlayerState* playerState) {
	int32 CurrentSlotNum = playerState->GetSlotNum();

	if (CurrentSlotNum != INDEX_NONE) {
		for (APlayerState* OtherPlayerState : PlayerArray) {
			if (const AFGPlayerState* OtherFGPlayerState = Cast<AFGPlayerState>(OtherPlayerState)) {
				if (OtherFGPlayerState != playerState && OtherFGPlayerState->GetSlotNum() == CurrentSlotNum) {
					CurrentSlotNum = INDEX_NONE;
					break;
				}
			}
		}
	}
	if (CurrentSlotNum != INDEX_NONE) {
		return CurrentSlotNum;
	}

	CurrentSlotNum = 0;
	//while(true) is really unnecessary here, but to preserve your sanity from raw goto I still added it
	outerLoopStart: while (true) {
		for (APlayerState* OtherPlayerState : PlayerArray) {
			if (const AFGPlayerState* OtherFGPlayerState = Cast<AFGPlayerState>(OtherPlayerState)) {
				if (OtherFGPlayerState != playerState &&
					OtherFGPlayerState->GetSlotNum() == CurrentSlotNum) {
					CurrentSlotNum++;
					goto outerLoopStart;
				}
			}
		}
		return CurrentSlotNum;
	}
}

bool AFGGameState::IsTradingPostBuilt() const {
	return mIsTradingPostBuilt;
}

bool AFGGameState::HasInitalTradingPostLandAnimPlayed() const {
	return mHasInitalTradingPostLandAnimPlayed;
}

void AFGGameState::SetHasInitalTradingPostLandAnimPlayed() {
	this->mHasInitalTradingPostLandAnimPlayed = true;
}

bool AFGGameState::IsSpaceElevatorBuilt() const {
	return this->mIsSpaceElevatorBuilt;
}

void AFGGameState::GetVisitedMapAreas(TArray<TSubclassOf<UFGMapArea>>& out_VisitedAreas) {
	out_VisitedAreas.Append(mVisitedMapAreas);
}

bool AFGGameState::IsMapAreaVisisted(TSubclassOf<UFGMapArea> inArea) {
	return mVisitedMapAreas.Contains(inArea);
}

void AFGGameState::AddUniqueVisistedMapArea(TSubclassOf< UFGMapArea > mapArea) {
	if (HasAuthority()) {
		if (!mVisitedMapAreas.Contains(mapArea)) {
			mVisitedMapAreas.AddUnique(mapArea);
			OnRep_MapAreaVisited();
		}
	}
}

void AFGGameState::OnRep_MapAreaVisited() {
	if (mVisitedMapAreas.Num()) {
		MapAreaVisistedDelegate.Broadcast(mVisitedMapAreas[mVisitedMapAreas.Num() - 1]);
	}
}

void AFGGameState::SetCheatNoPower(bool noPower) {
	this->mCheatNoPower = noPower;
	SubmitCheatTelemetry();
}

void AFGGameState::SetCheatNoCost(bool noCost) {
	this->mCheatNoCost = noCost;
	SubmitCheatTelemetry();
}

void AFGGameState::SetCheatNoFuel(bool noFuel) {
	this->mCheatNoFuel = noFuel;
	SubmitCheatTelemetry();
}

void AFGGameState::NotifyPlayerAdded(AFGCharacterPlayer* inPlayer) {
	if (mStorySubsystem) {
		mStorySubsystem->AddPlayer(inPlayer);
	}
	if (mTutorialIntroManager) {
		mTutorialIntroManager->AddPlayer(inPlayer);
	}
}

void AFGGameState::SendMessageToAllPlayers(TSubclassOf<UFGMessageBase> inMessage) {
	if (inMessage != NULL) {
		for (TPlayerControllerIterator<AFGPlayerController>::ServerAll It(GetWorld()); It; ++It) {
			It->Client_AddMessage(inMessage);
		}
	}
}

void AFGGameState::SendMessageToPlayer(TSubclassOf<UFGMessageBase> inMessage, APlayerController* controller) {
	if (inMessage != NULL) {
		if (AFGPlayerController* PlayerController = Cast<AFGPlayerController>(controller)) {
			PlayerController->Client_AddMessage(inMessage);
		}
	}
}

int32 AFGGameState::GetTotalPlayDuration() const {
	return mPlayDurationWhenLoaded + (int32)GetServerWorldTimeSeconds();
}

void AFGGameState::SetSessionName(const FString& inName) {
	this->mReplicatedSessionName = inName;
}

void AFGGameState::SetupColorSlots_Data(const TArray<FFactoryCustomizationColorSlot>& colorSlotsPrimary_Data) {
	if (!mHasInitializedColorSlots && HasAuthority()) {
		mBuildingColorSlots_Data = colorSlotsPrimary_Data;
		this->mHasInitializedColorSlots = true;
	}
}

void AFGGameState::Server_SetBuildingColorDataForSlot_Implementation(uint8 slotIdx, FFactoryCustomizationColorSlot colorData) {
	if (HasAuthority()) {
		ForceNetUpdate();
		mBuildingColorSlots_Data[slotIdx] = colorData;
		OnRep_BuildingColorSlot_Data();
	}
}

void AFGGameState::RemovePlayerColorPresetAtIndex(int32 index) {
	if (HasAuthority() && mPlayerGlobalColorPresets.IsValidIndex(index)) {
		mPlayerGlobalColorPresets.RemoveAt(index);
	}
}

void AFGGameState::AddPlayerColorPreset(FText presetName, FLinearColor color) {
	if (HasAuthority()) {
		FGlobalColorPreset ColorPreset{};
		ColorPreset.PresetName = presetName;
		ColorPreset.Color = color;
		mPlayerGlobalColorPresets.Add(ColorPreset);
	}
}

FLinearColor AFGGameState::GetBuildingColorPrimary_Linear(uint8 slot) {
	if (mBuildingColorSlots_Data.IsValidIndex(slot)) {
		return mBuildingColorSlots_Data[slot].PrimaryColor;
	}
	return FLinearColor::White;
}

FLinearColor AFGGameState::GetBuildingColorSecondary_Linear(uint8 slot) {
	if (mBuildingColorSlots_Data.IsValidIndex(slot)) {
		return mBuildingColorSlots_Data[slot].SecondaryColor;
	}
	return FLinearColor::White;
}

void AFGGameState::Server_SetBuildableLightColorSlot_Implementation(uint8 slotIdx, FLinearColor color) {
	if (HasAuthority() && mBuildableLightColorSlots.IsValidIndex(slotIdx)) {
		mBuildableLightColorSlots[slotIdx] = color;

		if (AFGBuildableSubsystem* BuildableSubsystem = AFGBuildableSubsystem::Get(this)) {
			BuildableSubsystem->BuildableLightColorSlotsUpdated(mBuildableLightColorSlots);
		}
	}
}

FFactoryCustomizationColorSlot AFGGameState::GetBuildingColorDataForSlot(uint8 slot) {
	if (mBuildingColorSlots_Data.IsValidIndex(slot)) {
		return mBuildingColorSlots_Data[slot];
	}
	return FFactoryCustomizationColorSlot{};
}

TSubclassOf<class UFGFactoryCustomizationDescriptor_Swatch> AFGGameState::GetCurrentSwatchForSwatchGroup(TSubclassOf<UFGSwatchGroup> swatchGroup) {
	for (const FSwatchGroupData& GroupData : mSwatchGroupDatum) {
		if (GroupData.SwatchGroup == swatchGroup) {
			return GroupData.Swatch;
		}
	}
	if (swatchGroup != UFGSwatchGroup_Standard::StaticClass()) {
		return GetCurrentSwatchForSwatchGroup(UFGSwatchGroup_Standard::StaticClass());
	}
	return NULL;
}

void AFGGameState::OnRep_BuildingColorSlot_Data(){ }

void AFGGameState::OnRep_BuildableLightColorSlots(){ }

void AFGGameState::ClaimPlayerColor( AFGPlayerState* playerState) {
	mAvailablePlayerColors
}

void AFGGameState::ReleasePlayerColor( AFGPlayerState* playerState){ }

void AFGGameState::ItemPickedUp(TSubclassOf<  UFGItemDescriptor > itemClass){ }

void AFGGameState::SetPlannedServerRestartWorldTime(float worldTimeSeconds){ }

FDateTime AFGGameState::GetServerLocalDateTime() const{ return FDateTime(); }

void AFGGameState::OnRep_OnlineSessionVisibility(){ }

bool AFGGameState::IsCustomizerRecipeUnlocked() {
	return false;
}

void AFGGameState::SetDefaultSwatchForBuildableGroup(TSubclassOf<  UFGSwatchGroup > swatchGroup, TSubclassOf<  UFGFactoryCustomizationDescriptor_Swatch> swatch) {
}

void AFGGameState::AddTetrominoResult(const FMiniGameResult& newResult) {
}

void AFGGameState::OnRep_TetrominoLeaderBoard() {
	mOnTetrominoLeaderBoardUpdated.Broadcast();
}

void AFGGameState::Server_SetPublicTodoList(const FString& newTodoList) {
	if (HasAuthority()) {
		this->mPublicTodoList = newTodoList;
		for (TPlayerControllerIterator<AFGPlayerController>::ServerAll It(GetWorld()); It; ++It) {
			if (AFGPlayerState* PlayerState = Cast<AFGPlayerState>(It->PlayerState)) {
				PlayerState->Client_UpdatePublicTodoList(newTodoList);
			}
		}
	}
}

void AFGGameState::CheckRestartTime() {
}

void AFGGameState::OnRep_PlannedRestartTime() {
	CheckRestartTime();
}

void AFGGameState::SubmitNumPlayersTelemetry() const {
}

void AFGGameState::SubmitCheatTelemetry() const {
}
