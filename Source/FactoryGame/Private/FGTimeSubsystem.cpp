#include "FGTimeSubsystem.h"

#include "FGGameMode.h"
#include "FGGameState.h"
#include "FGWorldSettings.h"
#include "Materials/MaterialParameterCollectionInstance.h"

AFGTimeOfDaySubsystem::AFGTimeOfDaySubsystem() : Super() {
	this->mSyncronizeTimeOfDayInterval = 5.0;
	this->mDayLengthMinutes = 3.0;
	this->mNightLengthMinutes = 1.5;
	this->mDayStartTime = 6.0;
	this->mNightStartTime = 18.0;
	this->mSpeedMultiplier = 1.0;
	this->mDaySeconds = 0.0;
	this->mReplicatedDaySeconds = 0.0;
	this->mNumberOfPassedDays = -1;
	this->mRTPCInterval = 20.0;
	this->mTimeOfDayMaterialParameterCollection = nullptr;
	this->mCachedTimeOfDayMaterialParameterCollection = nullptr;
	this->mUpdateTime = true;
	this->PrimaryActorTick.TickGroup = ETickingGroup::TG_PrePhysics;
	this->PrimaryActorTick.EndTickGroup = ETickingGroup::TG_PrePhysics;
	this->PrimaryActorTick.bTickEvenWhenPaused = false;
	this->PrimaryActorTick.bCanEverTick = true;
	this->PrimaryActorTick.bStartWithTickEnabled = true;
	this->PrimaryActorTick.bAllowTickOnDedicatedServer = true;
	this->PrimaryActorTick.TickInterval = 0.0;
}

void AFGTimeOfDaySubsystem::BeginPlay() {
	Super::BeginPlay();
	if (const AFGGameMode* GameMode = Cast<AFGGameMode>(GetWorld()->GetAuthGameMode())) {
		mDayLengthMinutes = GameMode->GetDayLength();
		mNightLengthMinutes = GameMode->GetNightLength();
	}

	if (GetWorld()->IsGameWorld() && HasAuthority()) {
		if (mNumberOfPassedDays < 0) {
			this->mNumberOfPassedDays = 0;

			if (const AFGWorldSettings* WorldSettings = Cast<AFGWorldSettings>(GetWorld()->GetWorldSettings())) {
				this->mDaySeconds = WorldSettings->GetStartingTimeOfDay() * SecondsPerHour;
			}
		}
		this->mReplicatedDaySeconds = mDaySeconds;

		if (mSyncronizeTimeOfDayInterval > 0.0f) {
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(mUpdateServerTimeTimer,
				FTimerDelegate::CreateUObject(this, &ThisClass::UpdateServerDaySeconds),
				mSyncronizeTimeOfDayInterval, true,
				mSyncronizeTimeOfDayInterval);
		}
	}

	if (mTimeOfDayMaterialParameterCollection) {
		this->mCachedTimeOfDayMaterialParameterCollection = GetWorld()->GetParameterCollectionInstance(mTimeOfDayMaterialParameterCollection);
	}
	//const float TimeOfDayHours = GetTimeOfDayHours();
	//UAkGameplayStatics::SetGlobalRTPCValue(NULL, TimeOfDayHours, 0, TEXT("TimeOfDay"));
}

void AFGTimeOfDaySubsystem::Tick(float dt) {
	Super::Tick(dt);

	if (!mUpdateTime) {
		return;
	}
	this->mDaySeconds += GetGameDeltaTime(dt);

	if (GetDaySeconds() >= SecondsPerDay) {
		this->mNumberOfPassedDays++;
		this->mDaySeconds = GetDaySeconds() - SecondsPerDay;
		this->mOnNewDayDelegate.Broadcast(mNumberOfPassedDays);
	}
	
	this->mRTPCAccumulator += dt;
	if (mRTPCAccumulator >= mRTPCInterval) {
		//const float TimeOfDayHours = GetTimeOfDayHours();
		//UAkGameplayStatics::SetGlobalRTPCValue(NULL, TimeOfDayHours, 0, TEXT("TimeOfDay"));
		this->mRTPCAccumulator = 0.0;
	}

	if (IsDay() && !mIsCurrentlyDay) {
		this->mIsCurrentlyDay = true;
		this->mOnDayStateDelegate.Broadcast(true);
	}
	else if (IsNight() && mIsCurrentlyDay) {
		this->mIsCurrentlyDay = false;
		this->mOnDayStateDelegate.Broadcast(false);
	}

	if (!mCachedTimeOfDayMaterialParameterCollection && GetWorld() && mTimeOfDayMaterialParameterCollection) {
		mCachedTimeOfDayMaterialParameterCollection = GetWorld()->GetParameterCollectionInstance(mTimeOfDayMaterialParameterCollection);
	}
	if (mCachedTimeOfDayMaterialParameterCollection) {
		mCachedTimeOfDayMaterialParameterCollection->SetScalarParameterValue(TEXT("TimeOfDayCyclePct"), GetDaySeconds() / (SecondsPerDay * 1.0f));
		mCachedTimeOfDayMaterialParameterCollection->SetScalarParameterValue(TEXT("DayPct"), GetDayPct());
		mCachedTimeOfDayMaterialParameterCollection->SetScalarParameterValue(TEXT("NightPct"), GetNightPct());
	}
}

void AFGTimeOfDaySubsystem::PreSaveGame_Implementation(int32 saveVersion, int32 gameVersion) {
}

void AFGTimeOfDaySubsystem::PostSaveGame_Implementation(int32 saveVersion, int32 gameVersion) {
}
void AFGTimeOfDaySubsystem::PreLoadGame_Implementation(int32 saveVersion, int32 gameVersion) {
}

void AFGTimeOfDaySubsystem::PostLoadGame_Implementation(int32 saveVersion, int32 gameVersion) {
}

void AFGTimeOfDaySubsystem::GatherDependencies_Implementation(TArray< UObject* >& out_dependentObjects) {
}

bool AFGTimeOfDaySubsystem::NeedTransform_Implementation() {
	return false;
}

bool AFGTimeOfDaySubsystem::ShouldSave_Implementation() const {
	return true;
}

void AFGTimeOfDaySubsystem::SetDaySeconds(float daySeconds) {
	this->mDaySeconds = daySeconds;
}

float AFGTimeOfDaySubsystem::GetNormalizedTimeOfDay() const {
	return GetDaySeconds() / (SecondsPerDay * 1.0f);
}

float AFGTimeOfDaySubsystem::GetDayPct() const {
	const float DayStartSeconds = mDayStartTime * SecondsPerHour;
	const float NightStartSeconds = mNightStartTime * SecondsPerHour;
	const float DaySeconds = GetDaySeconds();

	if (DayStartSeconds >= NightStartSeconds) {
		if (DaySeconds > NightStartSeconds && DaySeconds < DayStartSeconds) {
			return 0.0f;
		}
	} else {
		if (DaySeconds <= DayStartSeconds || DaySeconds >= NightStartSeconds) {
			return 0.0f;
		}
	}
	float SecondsSinceNightStart = DaySeconds - NightStartSeconds;
	if (SecondsSinceNightStart < 0.0f) {
		SecondsSinceNightStart += (SecondsPerDay - NightStartSeconds);
	}

	float NightDayIntervalLength;
	if (mDayStartTime >= mNightStartTime) {
		NightDayIntervalLength = 24.0f + mNightStartTime - mDayStartTime;
	} else {
		NightDayIntervalLength = mNightStartTime - mDayStartTime;
	}
	const float NightDayIntervalSeconds = NightDayIntervalLength * SecondsPerHour;
	return FMath::Sin(SecondsSinceNightStart / NightDayIntervalSeconds * PI);
}

float AFGTimeOfDaySubsystem::GetNightPct() const {
	const float DayStartSeconds = mDayStartTime * SecondsPerHour;
	const float NightStartSeconds = mNightStartTime * SecondsPerHour;
	const float DaySeconds = GetDaySeconds();

	if (DayStartSeconds >= NightStartSeconds) {
		if (DaySeconds <= NightStartSeconds || DaySeconds >= DayStartSeconds) {
			return 0.0f;
		}
	} else {
		if (DaySeconds > DayStartSeconds && DaySeconds < NightStartSeconds) {
			return 0.0f;
		}
	}
	float SecondsSinceDayStart = DaySeconds - DayStartSeconds;
	if (SecondsSinceDayStart < 0.0f) {
		SecondsSinceDayStart += (SecondsPerDay - DayStartSeconds);
	}

	float NightDayIntervalLength;
	if (mNightStartTime >= mDayStartTime) {
		NightDayIntervalLength = 24.0f + mDayStartTime - mNightStartTime;
	} else {
		NightDayIntervalLength = mDayStartTime - mNightStartTime;
	}
	const float NightDayIntervalSeconds = NightDayIntervalLength * SecondsPerHour;
	return FMath::Sin(SecondsSinceDayStart / NightDayIntervalSeconds * PI);
}

float AFGTimeOfDaySubsystem::GetTimeOfDayHours() const {
	return GetDaySeconds() / (SecondsPerHour * 1.0f);
}

float AFGTimeOfDaySubsystem::GetDaySeconds() const {
	return mDaySeconds;
}

float AFGTimeOfDaySubsystem::GetSeconds() const {
	const float DaySeconds = GetDaySeconds();
	return DaySeconds - (60.0f * (int32)((DaySeconds / (SecondsPerMinute * 1.0f) * 2) - 0.5f) / 2);
}

int32 AFGTimeOfDaySubsystem::GetDayMinutes() const {
	const float DaySeconds = GetDaySeconds();
	return (int32)((DaySeconds / (SecondsPerMinute * 1.0f) * 2) - 0.5f) / 2;
}

int32 AFGTimeOfDaySubsystem::GetMinutes() const {
	const float DaySeconds = GetDaySeconds();
	const float LastHourStartSeconds = SecondsPerHour * ((int32)(DaySeconds / (SecondsPerHour * 1.0f) * 2) / 2);
	return (int32)(((DaySeconds - LastHourStartSeconds) / (SecondsPerMinute * 1.0f) * 2) - 0.5f) / 2;
}

int32 AFGTimeOfDaySubsystem::GetHours() const {
	const float DaySeconds = GetDaySeconds();
	return (int32)((DaySeconds / (SecondsPerHour * 1.0f) * 2) - 0.5f) / 2;
}

int32 AFGTimeOfDaySubsystem::GetPassedDays() const {
	return mNumberOfPassedDays;
}

bool AFGTimeOfDaySubsystem::IsDay() const {
	const float DayStartSeconds = mDayStartTime * SecondsPerHour;
	const float NightStartSeconds = mNightStartTime * SecondsPerHour;
	const float DaySeconds = GetDaySeconds();

	if (DayStartSeconds >= NightStartSeconds) {
		if (DaySeconds > NightStartSeconds && DaySeconds < DayStartSeconds) {
			return false;
		}
	} else {
		if (DaySeconds <= DayStartSeconds || DaySeconds >= NightStartSeconds) {
			return false;
		}
	}
	return true;
}

bool AFGTimeOfDaySubsystem::IsNight() const {
	const float DayStartSeconds = mDayStartTime * SecondsPerHour;
	const float NightStartSeconds = mNightStartTime * SecondsPerHour;
	const float DaySeconds = GetDaySeconds();

	if (DayStartSeconds >= NightStartSeconds) {
		if (DaySeconds <= NightStartSeconds || DaySeconds >= DayStartSeconds) {
			return false;
		}
	} else {
		if (DaySeconds > DayStartSeconds && DaySeconds < NightStartSeconds) {
			return false;
		}
	}
	return true;
}

float AFGTimeOfDaySubsystem::GetDaytimeSeconds() const {
	float NightDayIntervalLength;
	if (mDayStartTime >= mNightStartTime) {
		NightDayIntervalLength = 24.0f + mNightStartTime - mDayStartTime;
	} else {
		NightDayIntervalLength = mNightStartTime - mDayStartTime;
	}
	return NightDayIntervalLength * SecondsPerHour;
}

float AFGTimeOfDaySubsystem::GetNighttimeSeconds() const {
	float NightDayIntervalLength;
	if (mNightStartTime >= mDayStartTime) {
		NightDayIntervalLength = 24.0f + mDayStartTime - mNightStartTime;
	} else {
		NightDayIntervalLength = mDayStartTime - mNightStartTime;
	}
	return NightDayIntervalLength * SecondsPerHour;
}

void AFGTimeOfDaySubsystem::ForceReplicateTimeToClients() {
	this->mReplicatedDaySeconds = mDaySeconds;
}

void AFGTimeOfDaySubsystem::SetTimeSpeedMultiplier(float multiplier) {
	this->mSpeedMultiplier = multiplier;
}

float AFGTimeOfDaySubsystem::GetDaytimeSpeed() const {
	float NightDayIntervalLength;
	if (mDayStartTime >= mNightStartTime) {
		NightDayIntervalLength = 24.0f + mNightStartTime - mDayStartTime;
	} else {
		NightDayIntervalLength = mNightStartTime - mDayStartTime;
	}
	return (60.0f / mDayLengthMinutes) * NightDayIntervalLength;
}

float AFGTimeOfDaySubsystem::GetNighttimeSpeed() const {
	float NightDayIntervalLength;
	if (mNightStartTime >= mDayStartTime) {
		NightDayIntervalLength = 24.0f + mDayStartTime - mNightStartTime;
	} else {
		NightDayIntervalLength = mDayStartTime - mNightStartTime;
	}
	return (60.0f / mNightLengthMinutes) * NightDayIntervalLength;
}

void AFGTimeOfDaySubsystem::OnRep_ReplicatedDaySeconds() {
	this->mServerDaySecondsDelta = this->mReplicatedDaySeconds - mDaySeconds;
}

void AFGTimeOfDaySubsystem::UpdateServerDaySeconds() {
	this->mReplicatedDaySeconds = mDaySeconds;
}

float AFGTimeOfDaySubsystem::GetAdjustedDaySeconds() const {
	float DaySeconds = GetDaySeconds() + mServerDaySecondsDelta;
	if (DaySeconds < 0.0f) {
		DaySeconds += SecondsPerDay;
	}
	return DaySeconds;
}

float AFGTimeOfDaySubsystem::GetGameDeltaTime(float dt) const {
	float DayTimeSpeed;
	if (IsDay()) {
		DayTimeSpeed = GetDaytimeSpeed();
	} else {
		DayTimeSpeed = GetNighttimeSeconds();
	}
	return (dt * DayTimeSpeed) * mSpeedMultiplier;
}

AFGTimeOfDaySubsystem* AFGTimeOfDaySubsystem::Get(const UObject* worldContext) {
	if (worldContext == NULL) {
		return NULL;
	}
	return Get(worldContext->GetWorld());
}

AFGTimeOfDaySubsystem* AFGTimeOfDaySubsystem::Get(UWorld* world) {
	if (world == NULL) {
		return NULL;
	}
	if (const AFGGameState* GameState = Cast<AFGGameState>(world->GetGameState())) {
		return GameState->GetTimeSubsystem();
	}
	return NULL;
}

void AFGTimeOfDaySubsystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFGTimeOfDaySubsystem, mDayLengthMinutes);
	DOREPLIFETIME(AFGTimeOfDaySubsystem, mNightLengthMinutes);
	DOREPLIFETIME(AFGTimeOfDaySubsystem, mDayStartTime);
	DOREPLIFETIME(AFGTimeOfDaySubsystem, mNightStartTime);
	DOREPLIFETIME(AFGTimeOfDaySubsystem, mSpeedMultiplier);
	DOREPLIFETIME(AFGTimeOfDaySubsystem, mReplicatedDaySeconds);
	DOREPLIFETIME(AFGTimeOfDaySubsystem, mNumberOfPassedDays);
	DOREPLIFETIME(AFGTimeOfDaySubsystem, mRTPCInterval);
	DOREPLIFETIME(AFGTimeOfDaySubsystem, mUpdateTime);
}
