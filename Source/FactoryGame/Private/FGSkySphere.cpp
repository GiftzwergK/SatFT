#include "FGSkySphere.h"
#include "FGAtmosphereUpdater.h"
#include "FGRainOcclusionActor.h"
#include "FGTimeSubsystem.h"
#include "FGWeatherReaction.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SceneComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialParameterCollectionInstance.h"

FSkySphereSettings::FSkySphereSettings() : SunIntensity(0.0f), MoonIntensity(0.0f),
	OverrideSunLightColor(false),
	OverrideSunIntensity(false),
	OverrideMoonLightColor(false),
	OverrideMoonIntensity(false) {
}

AFGSkySphere::AFGSkySphere() : Super() {
	this->mDefaultMaterial = nullptr;
	this->mDefaultLowSpecMaterial = nullptr;
	this->mDefaultMesh = nullptr;
	this->mSkyMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SkySphere"));
	this->mWeatherChangeDelayTime = FFloatInterval(3.40282e+38, -3.40282e+38);
	this->mRainOcclusionSceneCapture2DComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("OcclusionSceneCapture"));
	this->mRainOcclusionRT = nullptr;
	this->mRainOcclusionMaterialParameterCollection = nullptr;
	this->mTimeOfDayCollection = nullptr;
	this->mRainOcclusionActor = nullptr;
	this->mDefaultRainOcclusionActorClass = nullptr;
	this->mRainCaptureDistance = 10000.0;
	this->mSceneCaptureGridSnap = 0.0;
	this->mSunLightColorCurve.ExternalCurve = nullptr;
	this->mSunIntensity.EditorCurveData.DefaultValue = 4.0;
	this->mSunIntensity.EditorCurveData.PreInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mSunIntensity.EditorCurveData.PostInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mSunIntensity.ExternalCurve = nullptr;
	this->mSunRotationPitch.EditorCurveData.DefaultValue = 3.40282e+38;
	this->mSunRotationPitch.EditorCurveData.PreInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mSunRotationPitch.EditorCurveData.PostInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mSunRotationPitch.ExternalCurve = nullptr;
	this->mMoonLightColorCurve.ExternalCurve = nullptr;
	this->mMoonIntensity.EditorCurveData.DefaultValue = 3.40282e+38;
	this->mMoonIntensity.EditorCurveData.PreInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mMoonIntensity.EditorCurveData.PostInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mMoonIntensity.ExternalCurve = nullptr;
	this->mMoonRotationPitch.EditorCurveData.DefaultValue = 3.40282e+38;
	this->mMoonRotationPitch.EditorCurveData.PreInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mMoonRotationPitch.EditorCurveData.PostInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mMoonRotationPitch.ExternalCurve = nullptr;
	this->mOcclusionTintColor.ExternalCurve = nullptr;
	this->mSkyLightColor.ExternalCurve = nullptr;
	this->mCurrentLight = nullptr;
	this->mSkyLight = nullptr;
	this->mSunLight = nullptr;
	this->mMoonLight = nullptr;
	this->mClouds = nullptr;
	this->mSunOriginRotation = FRotator(0, 45, 25);
	this->mSunRotationAxis = FRotator(55, 190, 0);
	this->mMoonOriginRotation = FRotator(0, 45, 25);
	this->mMoonRotationAxis = FRotator(55, 190, 0);
	this->mDayTickRate = 0.1;
	this->mNightTickRate = 0.05;
	this->mCachedTimeOfDaySubSystem = nullptr;
	this->PrimaryActorTick.TickGroup = ETickingGroup::TG_PrePhysics;
	this->PrimaryActorTick.EndTickGroup = ETickingGroup::TG_PrePhysics;
	this->PrimaryActorTick.bTickEvenWhenPaused = false;
	this->PrimaryActorTick.bCanEverTick = true;
	this->PrimaryActorTick.bStartWithTickEnabled = true;
	this->PrimaryActorTick.bAllowTickOnDedicatedServer = true;
	this->PrimaryActorTick.TickInterval = 0.0;
	this->bReplicates = true;
	this->RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	this->mRainOcclusionSceneCapture2DComponent->SetupAttachment(RootComponent);
}

#if WITH_EDITOR

void AFGSkySphere::PostEditChangeChainProperty(FPropertyChangedChainEvent& propertyChangedEvent) {
	Super::PostEditChangeChainProperty(propertyChangedEvent);
	UpdatePreview();
}

float AFGSkySphere::GetViewMinInput() const {
	return mViewMinInput;
}

float AFGSkySphere::GetViewMaxInput() const {
	return mViewMaxInput;
}

void AFGSkySphere::SetViewRange(float min, float max) {
	this->mViewMinInput = min;
	this->mViewMaxInput = max;
}

#endif 

void AFGSkySphere::PostActorCreated() {
	Super::PostActorCreated();
#if WITH_EDITOR
	SetupPreviewDelegate();
#endif
}

void AFGSkySphere::PostLoad() {
	Super::PostLoad();
#if WITH_EDITOR
	SetupPreviewDelegate();
#endif
}

#if WITH_EDITOR

void AFGSkySphere::SetupPreviewDelegate() {
	AFGTimeOfDaySubsystem* TimeOfDaySubsystem = AFGTimeOfDaySubsystem::Get(this);
	this->mCachedTimeOfDaySubSystem = TimeOfDaySubsystem;

	if (TimeOfDaySubsystem) {
		this->mTimeOfDayUpdated = TimeOfDaySubsystem->GetTimeOfDayUpdatedDelegate().AddUObject(this, &ThisClass::UpdatePreview);
	}
}

#endif

void AFGSkySphere::BeginDestroy() {
	Super::BeginDestroy();
#if WITH_EDITOR
	if (mCachedTimeOfDaySubSystem && mTimeOfDayUpdated.IsValid()) {
		mCachedTimeOfDaySubSystem->GetTimeOfDayUpdatedDelegate().Remove(mTimeOfDayUpdated);
		mTimeOfDayUpdated.Reset();
	}
#endif
}

void AFGSkySphere::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (CanUpdate()) {
		UpdateLightRotation();
		UpdateCurvesFromTime();

		ADirectionalLight* NewCurrentLight = mMoonLight;
		if (NewCurrentLight->GetLightComponent()->Intensity <= mSunLight->GetLightComponent()->Intensity) {
			NewCurrentLight = mSunLight;
		}
		if (mCurrentLight != NewCurrentLight) {
			UpdateDominantLight(NewCurrentLight);
		}
		TryUpdateSceneCaptureLocation();
		
		if (mWeatherStateList.IsValidIndex(mCurrentSelectedWeather)) {
			if (mWeatherStateList[mCurrentSelectedWeather].bRequiresOcclusionRenderTarget) {
				mRainOcclusionSceneCapture2DComponent->CaptureScene();
			}
		}
	}

	if (HasAuthority() && GetWorld()->GetTimeSeconds() - mLastWeatherUpdateTime > mCurrentWeatherDelay && !bLockWeatherState) {
		if (mCachedTimeOfDaySubSystem) {
			GetNewWeatherState();
		}
	}
}

void AFGSkySphere::BeginPlay() {
	Super::BeginPlay();
	this->mCachedTimeOfDaySubSystem = AFGTimeOfDaySubsystem::Get(this);

	ADirectionalLight* NewCurrentLight = mMoonLight;
	if (NewCurrentLight->GetLightComponent()->Intensity <= mSunLight->GetLightComponent()->Intensity) {
		NewCurrentLight = mSunLight;
	}
	if (mCurrentLight != NewCurrentLight) {
		UpdateDominantLight(NewCurrentLight);
	}
	
	ADirectionalLight* SecondaryLight = NULL;
	if (mCurrentLight == mSunLight) {
		SecondaryLight = mMoonLight;
	} else if (mCurrentLight == mMoonLight) {
		SecondaryLight = mSunLight;
	}
	SecondaryLight->SetCastShadows(false);

	if (mSunLight) {
		mSunLight->SetActorRotation(mSunOriginRotation);
	}

	if (mWeatherStateList.Num()) {
		float TotalWeatherWeight = 0.0f;
		for (const FWeatherChanceEntry& WeatherChanceEntry : mWeatherStateList) {
			TotalWeatherWeight += WeatherChanceEntry.mProbability;
		}

		for (FWeatherChanceEntry& WeatherChanceEntry : mWeatherStateList) {
			WeatherChanceEntry.mProbability *= (100.0f / TotalWeatherWeight);
		}
		mWeatherReactionActors.SetNumZeroed(mWeatherStateList.Num());

		mWeatherSeed.GenerateNewSeed();
		mLastWeatherUpdateTime = GetWorld()->GetTimeSeconds();

		FActorSpawnParameters SpawnParameters{};
		SpawnParameters.bNoFail = true;
		this->mRainOcclusionActor = GetWorld()->SpawnActor<AFGRainOcclusionActor>(mDefaultRainOcclusionActorClass, SpawnParameters);

		mRainOcclusionSceneCapture2DComponent->TextureTarget = mRainOcclusionRT;

		UMaterialParameterCollectionInstance* ParameterCollectionInstance = GetWorld()->GetParameterCollectionInstance(mRainOcclusionMaterialParameterCollection);
		if (ParameterCollectionInstance) {
			ParameterCollectionInstance->SetScalarParameterValue(TEXT("RTOcclusionDistance"), mRainCaptureDistance);
		}
		mRainOcclusionSceneCapture2DComponent->OrthoWidth = mRainCaptureDistance;
		mRainOcclusionSceneCapture2DComponent->CaptureScene();
	}
}

void AFGSkySphere::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFGSkySphere, mCurrentSelectedWeather);
}

void AFGSkySphere::OnConstruction(const FTransform& Transform) {
	Super::OnConstruction(Transform);
	mSkyMeshComponent->SetStaticMesh(mDefaultMesh);
}

void AFGSkySphere::UpdateMaterial(bool bWithVolumetricClouds) {
	if (bWithVolumetricClouds) {
		mSkyMeshComponent->SetMaterial(0, mDefaultMaterial);
	} else {
		mSkyMeshComponent->SetMaterial(0, mDefaultLowSpecMaterial);
	}
}

void AFGSkySphere::SetDirectionalLightIntensityMultiplierOverride(float multiplier) {
	if (UFGAtmosphereUpdater* AtmosphereUpdater = UFGAtmosphereUpdater::AtmosphereUpdaterStatic) {
		AtmosphereUpdater->mDirectionalLightMultiplierOverride = FMath::Clamp(multiplier, 0.0f, 1.0f);
	}
}

float AFGSkySphere::GetDirectionalLightIntensityMultiplierOverride() {
	if (UFGAtmosphereUpdater* AtmosphereUpdater = UFGAtmosphereUpdater::AtmosphereUpdaterStatic) {
		return AtmosphereUpdater->mDirectionalLightMultiplierOverride;
	}
	return 1.0f;
}

FWeatherChanceEntry AFGSkySphere::GetNewWeatherState() {
	if (mWeatherStateList.Num() <= 1) {
		if (mWeatherStateList.Num()) {
			SetWeatherState(mDefaultWeatherEntry);
			return mWeatherStateList[mDefaultWeatherEntry];
		}
		return FWeatherChanceEntry();
	}

	const float RandomWeatherRoll = FMath::FRandRange(0.0f, 100.0f);
	float CurrentWeatherProbability = 0.0f;

	int32 RolledWeatherIndex = mDefaultWeatherEntry;
	
	for (int32 i = 0; i < mWeatherStateList.Num(); i++) {
		const FWeatherChanceEntry& WeatherChanceEntry = mWeatherStateList[i];
		const float NewCurrentWeatherProbability = CurrentWeatherProbability + WeatherChanceEntry.mProbability;

		if (RandomWeatherRoll <= NewCurrentWeatherProbability &&
			RandomWeatherRoll >= CurrentWeatherProbability &&
			mCurrentSelectedWeather != i) {

			RolledWeatherIndex = i;
			if (mCachedTimeOfDaySubSystem->GetPassedDays() >= WeatherChanceEntry.mWeatherStartupDelayDays) {
				break;
			}
		}
		CurrentWeatherProbability = NewCurrentWeatherProbability;
	}
	SetWeatherState(RolledWeatherIndex);
	return mWeatherStateList[RolledWeatherIndex];
}

void AFGSkySphere::SetWeatherState(int32 NewTypeID) {
	const int32 CurrentSelectedWeather = mCurrentSelectedWeather;
	if (CurrentSelectedWeather != NewTypeID) {
		this->mCurrentSelectedWeather = NewTypeID;
		const FWeatherChanceEntry& WeatherChanceEntry = mWeatherStateList[NewTypeID];

		this->mCachedSeedResult = mWeatherSeed.FRand();
		this->mLastWeatherUpdateTime = GetWorld()->GetTimeSeconds();
		this->mCurrentWeatherDelay = WeatherChanceEntry.mWeatherEffectDuration.Interpolate(mCachedSeedResult);

		OnRep_OnWeatherChanged(CurrentSelectedWeather);
		FlushNetDormancy();
	}
}

void AFGSkySphere::OnRep_OnWeatherChanged(int32 OldState) {
	if (mWeatherReactionActors.IsValidIndex(OldState)) {
		AFGWeatherReaction* OldReactionActor = mWeatherReactionActors[OldState].Get();
		if (OldReactionActor) {
			OldReactionActor->EndWeatherState(this);
		}
	}
	if (mWeatherReactionActors.IsValidIndex(mCurrentSelectedWeather)) {
		AFGWeatherReaction* NewReactionActor = mWeatherReactionActors[mCurrentSelectedWeather].Get();
		if (NewReactionActor == NULL) {
			const FActorSpawnParameters ActorSpawnParameters{};
			const TSubclassOf<AFGWeatherReaction> WeatherReactionClass = mWeatherStateList[mCurrentSelectedWeather].mWeatherBehaviour;

			NewReactionActor = GetWorld()->SpawnActor<AFGWeatherReaction>(WeatherReactionClass, FTransform::Identity, ActorSpawnParameters);
			mWeatherReactionActors[mCurrentSelectedWeather] = NewReactionActor;
		}
		if (NewReactionActor) {
			NewReactionActor->StartWeatherState(this);
		}
	}
}

void AFGSkySphere::SetCloudShadowIntensity(float NewValue) {
	if (mMoonLight) {
		if (UDirectionalLightComponent* DirectionalLightComponent = Cast<UDirectionalLightComponent>(mMoonLight->GetLightComponent())) {
			DirectionalLightComponent->CloudShadowStrength = NewValue;
		}
	}
	if (mSunLight) {
		if (UDirectionalLightComponent* DirectionalLightComponent = Cast<UDirectionalLightComponent>(mSunLight->GetLightComponent())) {
			DirectionalLightComponent->CloudShadowStrength = NewValue;
		}
	}
}

float AFGSkySphere::GetCloudShadowIntensity() const {
	if (mMoonLight) {
		if (const UDirectionalLightComponent* DirectionalLightComponent = Cast<UDirectionalLightComponent>(mMoonLight->GetLightComponent())) {
			return DirectionalLightComponent->CloudShadowStrength;
		}
	}
	return 0.0f;
}

void AFGSkySphere::TryUpdateSceneCaptureLocation(bool bForce) {
	if (mRainOcclusionSceneCapture2DComponent) {
		const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		FVector OutViewPointLocation;
		FRotator OutViewPointRotation;
		PlayerController->GetPlayerViewPoint(OutViewPointLocation, OutViewPointRotation);

		const float SnappedPosX = FMath::GridSnap(OutViewPointLocation.X, mSceneCaptureGridSnap);
		const float SnappedPosY = FMath::GridSnap(OutViewPointLocation.Y, mSceneCaptureGridSnap);

		const FVector CurrentLocation = mRainOcclusionSceneCapture2DComponent->GetComponentLocation();
		if (bForce || (FMath::Abs(CurrentLocation.X - SnappedPosX) > 0.1f || FMath::Abs(CurrentLocation.Y - SnappedPosY) > 0.1f)) {

			FVector ResultLocation;
			ResultLocation.X = SnappedPosX;
			ResultLocation.Y = SnappedPosY;
			ResultLocation.Z = OutViewPointLocation.Z + 50000.0;

			mRainOcclusionSceneCapture2DComponent->SetWorldLocation(ResultLocation);

			UMaterialParameterCollectionInstance* ParameterCollectionInstance = GetWorld()->GetParameterCollectionInstance(mRainOcclusionMaterialParameterCollection);
			if (ParameterCollectionInstance) {
				ParameterCollectionInstance->SetVectorParameterValue(TEXT("RTLocationName"), FLinearColor(ResultLocation));
				mRainOcclusionSceneCapture2DComponent->OrthoWidth = mRainCaptureDistance;
			}
			if (mRainOcclusionActor) {
				mRainOcclusionActor->ForceUpdateOcclusionShapes(ResultLocation, mRainCaptureDistance);
			}
		}
	}
}

void AFGSkySphere::UpdateOcclusionDistance() {
	if (UMaterialParameterCollectionInstance* ParameterCollectionInstance = GetWorld()->GetParameterCollectionInstance(mRainOcclusionMaterialParameterCollection)) {
		ParameterCollectionInstance->SetScalarParameterValue(TEXT("RTOcclusionDistance"), mRainCaptureDistance);
	}
	mRainOcclusionSceneCapture2DComponent->OrthoWidth = mRainCaptureDistance;
}

void AFGSkySphere::UpdateLightRotation() {
	if (mSunLight) {
		const FRotator NewSunRotation = CalculateLightRotation(mSunOriginRotation, mSunRotationAxis, mSunRotationPitch);
		mSunLight->SetActorRotation(NewSunRotation);
	}
	if (mMoonLight) {
		const FRotator NewMoonRotation = CalculateLightRotation(mMoonOriginRotation, mMoonRotationAxis, mMoonRotationPitch);
		mMoonLight->SetActorRotation(NewMoonRotation);
	}
}

void AFGSkySphere::UpdateCurvesFromTime() {
	const float LinearTime = mCachedTimeOfDaySubSystem->GetDaySeconds() / (AFGTimeOfDaySubsystem::SecondsPerDay * 1.0f);
	if (mSkyLight) {
		const FLinearColor NewSkyColor = mSkyLightColor.GetLinearColorValue(LinearTime);
		mSkyLight->GetLightComponent()->SetLightColor(NewSkyColor);
		mSkyLight->GetLightComponent()->OcclusionTint = mOcclusionTintColor.GetLinearColorValue(LinearTime).ToFColor(false);
	}
}

void AFGSkySphere::CalculateDominantLight() {
	ADirectionalLight* NewCurrentLight = mMoonLight;
	if (NewCurrentLight->GetLightComponent()->Intensity <= mSunLight->GetLightComponent()->Intensity) {
		NewCurrentLight = mSunLight;
	}
	if (mCurrentLight != NewCurrentLight) {
		UpdateDominantLight(NewCurrentLight);
	}
}

void AFGSkySphere::UpdateDominantLight(ADirectionalLight* NewDominantLight) {
	if (mCurrentLight) {
		mCurrentLight->SetCastShadows(false);
	}
	this->mCurrentLight = NewDominantLight;
	NewDominantLight->SetCastShadows(true);

	if (NewDominantLight == mSunLight) {
		mSunLight->SetActorTickInterval(mDayTickRate);
	} else {
		mMoonLight->SetActorTickInterval(mNightTickRate);
	}
}

FRotator AFGSkySphere::CalculateLightRotation(FRotator OriginalRotation, FRotator RotationAxis, FRuntimeFloatCurve LightRotationCurve) const {
	const float LinearTime = mCachedTimeOfDaySubSystem->GetDaySeconds() / (AFGTimeOfDaySubsystem::SecondsPerDay * 1.0f);
	const float RotationAngle = GetFloatCurveValue(LightRotationCurve, LinearTime);

	const FVector OriginalRotationVector = RotationAxis.Vector();
	const FVector ForwardDirection = OriginalRotation.UnrotateVector(OriginalRotationVector);

	const FRotator ForwardRotator = FRotationMatrix::MakeFromX(ForwardDirection).Rotator();
	const FRotator RotationAnglePitchRotator(RotationAngle * -1.0f, 0.0f, 0.0f);
	
	const FVector ResultRotationDirection = UKismetMathLibrary::ComposeRotators(ForwardRotator, RotationAnglePitchRotator).Vector();
	const FVector ResultRotation = OriginalRotation.RotateVector(ResultRotationDirection);

	return FRotationMatrix::MakeFromX(ResultRotation).Rotator();
}

bool AFGSkySphere::CanUpdate() {
	if (mIsInitialized) {
		return true;
	}
	this->mCachedTimeOfDaySubSystem = AFGTimeOfDaySubsystem::Get(this);
	if (mSunLight == NULL || mSunLight->IsPendingKill()) {
		return false;
	}
	if (mMoonLight == NULL || mMoonLight->IsPendingKill()) {
		return false;
	}
	if (mSkyLight == NULL || mSkyLight->IsPendingKill()) {
		return false;
	}
	if (mClouds == NULL || mClouds->IsPendingKill()) {
		return false;
	}
	if (mCachedTimeOfDaySubSystem == NULL || mCachedTimeOfDaySubSystem->IsPendingKill()) {
		return false;
	}
	this->mIsInitialized = true;
	return true;
}

bool AFGSkySphere::CanUpdatePreview() const {
	if (mSunLight == NULL || mSunLight->IsPendingKill()) {
		return false;
	}
	if (mMoonLight == NULL || mMoonLight->IsPendingKill()) {
		return false;
	}
	if (mSkyLight == NULL || mSkyLight->IsPendingKill()) {
		return false;
	}
	if (mClouds == NULL || mClouds->IsPendingKill()) {
		return false;
	}
	return true;
}

void AFGSkySphere::ApplySkySphereSettings_Implementation(const FSkySphereSettings& settings) {
	if (mSunLight) {
		mSunLight->SetLightColor(settings.SunLightColor);
		mSunLight->GetLightComponent()->SetIntensity(settings.SunIntensity);
	}
	if (mMoonLight) {
		mMoonLight->SetLightColor(settings.MoonLightColor);
		mMoonLight->GetLightComponent()->SetIntensity(settings.MoonIntensity);
	}
}

void AFGSkySphere::UpdatePreview() {
	if (CanUpdatePreview()) {
		UpdateLightRotation();
		UpdateCurvesFromTime();

		ADirectionalLight* NewCurrentLight = mMoonLight;
		if (NewCurrentLight->GetLightComponent()->Intensity <= mSunLight->GetLightComponent()->Intensity) {
			NewCurrentLight = mSunLight;
		}
		if (mCurrentLight != NewCurrentLight) {
			UpdateDominantLight(NewCurrentLight);
		}
	}
}

FLinearColor AFGSkySphere::GetColorCurveValue(const FRuntimeCurveLinearColor& curve, float time) {
	return curve.GetLinearColorValue(time);
}

float AFGSkySphere::GetFloatCurveValue(const FRuntimeFloatCurve& curve, float time) {
	return curve.GetRichCurveConst()->Eval(time);
}

void AFGSkySphere::GetSkySphereSettings(float atTime, FSkySphereSettings& out_settings) const {
	out_settings.OverrideSunIntensity = true;
	out_settings.OverrideSunLightColor = true;
	out_settings.SunIntensity = GetFloatCurveValue(mSunIntensity, atTime);
	out_settings.SunLightColor = GetColorCurveValue(mSunLightColorCurve, atTime);

	out_settings.OverrideMoonIntensity = true;
	out_settings.OverrideMoonLightColor = true;
	out_settings.MoonIntensity = GetFloatCurveValue(mMoonIntensity, atTime);
	out_settings.MoonLightColor = GetColorCurveValue(mMoonLightColorCurve, atTime);
}

bool AFGSkySphere::DoesWeatherEffectNeedOcclusion(const FWeatherChanceEntry& Type) {
	return Type.bRequiresOcclusionRenderTarget;
}

void AFGSkySphere::UpdateGlobalMaterialCollection() {
	checkf(false, TEXT("Deprecated"));
}
