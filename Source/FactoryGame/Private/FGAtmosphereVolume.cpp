#include "FGAtmosphereVolume.h"
#include "FGTimeSubsystem.h"
#include "FGWorldSettings.h"
#include "Atmosphere/BiomeHelpers.h"
#include "Atmosphere/UFGBiome.h"
#include "Components/SkyAtmosphereComponent.h"

FExponentialFogSettings::FExponentialFogSettings() {
	this->FogHeight = 5000.0;
	this->FogDensity = 0.02;
	this->FullyDirectionalInscatteringColorDistance = 100000.0f;
	this->NonDirectionalInscatteringColorDistance = 1000.0f;
	this->FogInscatteringColor = FLinearColor(0.0f, 0.0f, 0.0f);
	this->DirectionalInscatteringExponent = 4.0f;
	this->DirectionalInscatteringStartDistance = 10000.0f;
	this->FogCutoffDistance = 0.0f;
	this->SecondFogDensity = 0.0f;
	this->DirectionalInscatteringColor = FLinearColor(0.0f, 0.0f, 0.0f);
	this->FogHeightFalloff = 0.2;
	this->FogMaxOpacity = 1.0f;
	this->StartDistance = 0.0f;
	this->SecondFogHeightFalloff = 0.2f;
	this->SecondFogHeightOffset = 0.0f;
	this->EnableFogHeight = false;
	this->EnableFogDensity = false;
	this->EnableFogInscatteringColor = false;
	this->EnableFullyDirectionalInscatteringColorDistance = false;
	this->EnableNonDirectionalInscatteringColorDistance = false;
	this->EnableDirectionalInscatteringExponent = false;
	this->EnableDirectionalInscatteringStartDistance = false;
	this->EnableDirectionalInscatteringColor = false;
	this->EnableFogHeightFalloff = false;
	this->EnableFogMaxOpacity = false;
	this->EnableStartDistance = false;
	this->EnableFogCutoffDistance = false;
	this->EnableSecondFogDensity = false;
	this->EnableSecondFogHeightFalloff = false;
	this->EnableSecondFogHeightOffset = false;
}

FSkyAtmosphereSettings::FSkyAtmosphereSettings() {
	const USkyAtmosphereComponent* SkyAtmosphereComponent = GetDefault<ASkyAtmosphere>()->GetComponent();
	
	this->RayleighScatteringScale = SkyAtmosphereComponent->RayleighScatteringScale;
	this->RayleighScattering = SkyAtmosphereComponent->RayleighScattering;
	this->RayleighExponentialDistribution = SkyAtmosphereComponent->RayleighExponentialDistribution;
	this->MieScatteringScale = SkyAtmosphereComponent->MieScatteringScale;
	this->MieScattering = SkyAtmosphereComponent->MieScattering;
	this->MieAbsorptionScale = SkyAtmosphereComponent->MieAbsorptionScale;
	this->MieAbsorption = SkyAtmosphereComponent->MieAbsorption;
	this->MieAnisotropy = SkyAtmosphereComponent->MieAnisotropy;
	this->MieExponentialDistribution = SkyAtmosphereComponent->MieExponentialDistribution;
	this->OtherAbsorptionScale = SkyAtmosphereComponent->OtherAbsorptionScale;
	this->OtherAbsorption = SkyAtmosphereComponent->OtherAbsorption;
	
	this->OverrideRayleighScatteringScale = false;
	this->OverrideRayleighScattering = false;
	this->OverrideRayleighExponentialDistribution = false;
	this->OverrideMieScatteringScale = false;
	this->OverrideMieScattering = false;
	this->OverrideMieAbsorptionScale = false;
	this->OverrideMieAbsorption = false;
	this->OverrideMieAnisotropy = false;
	this->OverrideMieExponentialDistribution = false;
	this->OverrideOtherAbsorptionScale = false;
	this->OverrideOtherAbsorption = false;
}

AFGAtmosphereVolume::AFGAtmosphereVolume() : Super() {
	this->mBiome = NewObject<UFGBiome>(this, TEXT("Biome"));
	this->mPriority = 1.0;
	this->mBlendDistance = 100.0;
	this->mFogHeight.EditorCurveData.DefaultValue = 5000.0;
	this->mFogHeight.EditorCurveData.PreInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mFogHeight.EditorCurveData.PostInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mFogHeight.ExternalCurve = nullptr;
	this->mFogDensity.EditorCurveData.DefaultValue = 0.02;
	this->mFogDensity.EditorCurveData.PreInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mFogDensity.EditorCurveData.PostInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mFogDensity.ExternalCurve = nullptr;
	this->mFogInscatteringColor.ExternalCurve = nullptr;
	this->mFullyDirectionalInscatteringColorDistance.EditorCurveData.DefaultValue = 100000.0;
	this->mFullyDirectionalInscatteringColorDistance.EditorCurveData.PreInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mFullyDirectionalInscatteringColorDistance.EditorCurveData.PostInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mFullyDirectionalInscatteringColorDistance.ExternalCurve = nullptr;
	this->mNonDirectionalInscatteringColorDistance.EditorCurveData.DefaultValue = 1000.0;
	this->mNonDirectionalInscatteringColorDistance.EditorCurveData.PreInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mNonDirectionalInscatteringColorDistance.EditorCurveData.PostInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mNonDirectionalInscatteringColorDistance.ExternalCurve = nullptr;
	this->mDirectionalInscatteringExponent.EditorCurveData.DefaultValue = 4.0;
	this->mDirectionalInscatteringExponent.EditorCurveData.PreInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mDirectionalInscatteringExponent.EditorCurveData.PostInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mDirectionalInscatteringExponent.ExternalCurve = nullptr;
	this->mDirectionalInscatteringStartDistance.EditorCurveData.DefaultValue = 10000.0;
	this->mDirectionalInscatteringStartDistance.EditorCurveData.PreInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mDirectionalInscatteringStartDistance.EditorCurveData.PostInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mDirectionalInscatteringStartDistance.ExternalCurve = nullptr;
	this->mDirectionalInscatteringColor.ExternalCurve = nullptr;
	this->mFogHeightFalloff.EditorCurveData.DefaultValue = 0.2;
	this->mFogHeightFalloff.EditorCurveData.PreInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mFogHeightFalloff.EditorCurveData.PostInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mFogHeightFalloff.ExternalCurve = nullptr;
	this->mFogMaxOpacity.EditorCurveData.DefaultValue = 1.0;
	this->mFogMaxOpacity.EditorCurveData.PreInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mFogMaxOpacity.EditorCurveData.PostInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mFogMaxOpacity.ExternalCurve = nullptr;
	this->mStartDistance.EditorCurveData.DefaultValue = 0.0;
	this->mStartDistance.EditorCurveData.PreInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mStartDistance.EditorCurveData.PostInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mStartDistance.ExternalCurve = nullptr;
	this->mFogCutoffDistance.EditorCurveData.DefaultValue = 0.0;
	this->mFogCutoffDistance.EditorCurveData.PreInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mFogCutoffDistance.EditorCurveData.PostInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mFogCutoffDistance.ExternalCurve = nullptr;
	this->mHorizonColorCurve.ExternalCurve = nullptr;
	this->mZenithColorCurve.ExternalCurve = nullptr;
	this->mCloudColorCurve.ExternalCurve = nullptr;
	this->mSunLightColorCurve.ExternalCurve = nullptr;
	this->mMoonLightColorCurve.ExternalCurve = nullptr;
	this->mSunIntensity.EditorCurveData.DefaultValue = 3.40282e+38;
	this->mSunIntensity.EditorCurveData.PreInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mSunIntensity.EditorCurveData.PostInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mSunIntensity.ExternalCurve = nullptr;
	this->mMoonIntensity.EditorCurveData.DefaultValue = 3.40282e+38;
	this->mMoonIntensity.EditorCurveData.PreInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mMoonIntensity.EditorCurveData.PostInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mMoonIntensity.ExternalCurve = nullptr;
	this->mCloudOpacity.EditorCurveData.DefaultValue = 3.40282e+38;
	this->mCloudOpacity.EditorCurveData.PreInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mCloudOpacity.EditorCurveData.PostInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mCloudOpacity.ExternalCurve = nullptr;
	this->mBlendWeight = 1.0;
	this->mWhiteTemp.EditorCurveData.DefaultValue = 3.40282e+38;
	this->mWhiteTemp.EditorCurveData.PreInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mWhiteTemp.EditorCurveData.PostInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mWhiteTemp.ExternalCurve = nullptr;
	this->mWhiteTint.EditorCurveData.DefaultValue = 3.40282e+38;
	this->mWhiteTint.EditorCurveData.PreInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mWhiteTint.EditorCurveData.PostInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mWhiteTint.ExternalCurve = nullptr;
	this->mColorSaturation.ExternalCurve = nullptr;
	this->mColorContrast.ExternalCurve = nullptr;
	this->mColorGamma.ExternalCurve = nullptr;
	this->mColorGain.ExternalCurve = nullptr;
	this->mColorOffset.ExternalCurve = nullptr;
	this->mColorSaturationShadows.ExternalCurve = nullptr;
	this->mColorContrastShadows.ExternalCurve = nullptr;
	this->mColorGammaShadows.ExternalCurve = nullptr;
	this->mColorGainShadows.ExternalCurve = nullptr;
	this->mColorOffsetShadows.ExternalCurve = nullptr;
	this->mColorSaturationMidtones.ExternalCurve = nullptr;
	this->mColorContrastMidtones.ExternalCurve = nullptr;
	this->mColorGammaMidtones.ExternalCurve = nullptr;
	this->mColorGainMidtones.ExternalCurve = nullptr;
	this->mColorOffsetMidtones.ExternalCurve = nullptr;
	this->mColorSaturationHighlights.ExternalCurve = nullptr;
	this->mColorContrastHighlights.ExternalCurve = nullptr;
	this->mColorGammaHighlights.ExternalCurve = nullptr;
	this->mColorGainHighlights.ExternalCurve = nullptr;
	this->mColorOffsetHighlights.ExternalCurve = nullptr;
	this->mColorCorrectionHighlightsMin.EditorCurveData.DefaultValue = 3.40282e+38;
	this->mColorCorrectionHighlightsMin.EditorCurveData.PreInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mColorCorrectionHighlightsMin.EditorCurveData.PostInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mColorCorrectionHighlightsMin.ExternalCurve = nullptr;
	this->mColorCorrectionShadowsMax.EditorCurveData.DefaultValue = 3.40282e+38;
	this->mColorCorrectionShadowsMax.EditorCurveData.PreInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mColorCorrectionShadowsMax.EditorCurveData.PostInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mColorCorrectionShadowsMax.ExternalCurve = nullptr;
	this->mBlueCorrection.EditorCurveData.DefaultValue = 3.40282e+38;
	this->mBlueCorrection.EditorCurveData.PreInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mBlueCorrection.EditorCurveData.PostInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mBlueCorrection.ExternalCurve = nullptr;
	this->mExpandGamut.EditorCurveData.DefaultValue = 3.40282e+38;
	this->mExpandGamut.EditorCurveData.PreInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mExpandGamut.EditorCurveData.PostInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mExpandGamut.ExternalCurve = nullptr;
	this->mSceneColorTint.ExternalCurve = nullptr;
	this->mFilmSlope.EditorCurveData.DefaultValue = 3.40282e+38;
	this->mFilmSlope.EditorCurveData.PreInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mFilmSlope.EditorCurveData.PostInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mFilmSlope.ExternalCurve = nullptr;
	this->mFilmToe.EditorCurveData.DefaultValue = 3.40282e+38;
	this->mFilmToe.EditorCurveData.PreInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mFilmToe.EditorCurveData.PostInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mFilmToe.ExternalCurve = nullptr;
	this->mFilmShoulder.EditorCurveData.DefaultValue = 3.40282e+38;
	this->mFilmShoulder.EditorCurveData.PreInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mFilmShoulder.EditorCurveData.PostInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mFilmShoulder.ExternalCurve = nullptr;
	this->mFilmBlackClip.EditorCurveData.DefaultValue = 3.40282e+38;
	this->mFilmBlackClip.EditorCurveData.PreInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mFilmBlackClip.EditorCurveData.PostInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mFilmBlackClip.ExternalCurve = nullptr;
	this->mFilmWhiteClip.EditorCurveData.DefaultValue = 3.40282e+38;
	this->mFilmWhiteClip.EditorCurveData.PreInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mFilmWhiteClip.EditorCurveData.PostInfinityExtrap = ERichCurveExtrapolation::RCCE_Constant;
	this->mFilmWhiteClip.ExternalCurve = nullptr;
	this->mEnableFogHeight = false;
	this->mEnableFogDensity = false;
	this->mEnableFogInscatteringColor = false;
	this->mEnableFullyDirectionalInscatteringColorDistance = false;
	this->mEnableNonDirectionalInscatteringColorDistance = false;
	this->mEnableDirectionalInscatteringExponent = false;
	this->mEnableDirectionalInscatteringStartDistance = false;
	this->mEnableDirectionalInscatteringColor = false;
	this->mEnableFogHeightFalloff = false;
	this->mEnableFogMaxOpacity = false;
	this->mEnableStartDistance = false;
	this->mEnableFogCutoffDistance = false;
	this->mOverrideHorizonColor = false;
	this->mOverrideMoonIntensity = false;
	this->mOverrideCloudOpacity = false;
	this->mOverrideSunIntensity = false;
	this->mOverrideZenithColor = false;
	this->mOverrideCloudColor = false;
	this->mOverrideSunLightColor = false;
	this->mOverrideMoonLightColor = false;
	this->mEnableWhiteTemp = false;
	this->mEnableWhiteTint = false;
	this->mEnableColorSaturation = false;
	this->mEnableColorContrast = false;
	this->mEnableColorGamma = false;
	this->mEnableColorGain = false;
	this->mEnableColorOffset = false;
	this->mEnableFilmSlope = false;
	this->mEnableFilmToe = false;
	this->mEnableFilmShoulder = false;
	this->mEnableFilmBlackClip = false;
	this->mEnableFilmWhiteClip = false;
	this->mEnableColorSaturationShadows = false;
	this->mEnableColorContrastShadows = false;
	this->mEnableColorGammaShadows = false;
	this->mEnableColorGainShadows = false;
	this->mEnableColorOffsetShadows = false;
	this->mEnableColorSaturationMidtones = false;
	this->mEnableColorContrastMidtones = false;
	this->mEnableColorGammaMidtones = false;
	this->mEnableColorGainMidtones = false;
	this->mEnableColorOffsetMidtones = false;
	this->mEnableColorSaturationHighlights = false;
	this->mEnableColorContrastHighlights = false;
	this->mEnableColorGammaHighlights = false;
	this->mEnableColorGainHighlights = false;
	this->mEnableColorOffsetHighlights = false;
	this->mEnableColorCorrectionShadowsMax = false;
	this->mEnableColorCorrectionHighlightsMin = false;
	this->mEnableBlueCorrection = false;
	this->mEnableExpandGamut = false;
	this->mEnableSceneColorTint = false;
}

#if WITH_EDITOR

void AFGAtmosphereVolume::PostEditChangeProperty(FPropertyChangedEvent& propertyChangedEvent) {
	Super::PostEditChangeProperty(propertyChangedEvent);
	if (mBiome != NULL) {
		GetSettings(mPreviewSettings);
	}
}

void AFGAtmosphereVolume::MoveCurvesToBaseBiome() {
}

void AFGAtmosphereVolume::CopyLegacyPropertiesToAsset() {
}

#endif 

void AFGAtmosphereVolume::BeginPlay() {
	Super::BeginPlay();
}

void AFGAtmosphereVolume::PostLoad() {
	Super::PostLoad();
}

void AFGAtmosphereVolume::PostRegisterAllComponents() {
	Super::PostRegisterAllComponents();
	if (UWorld* World = GetWorld()) {
		World->InsertPostProcessVolume(this);
		AddVolume();
	}
}

void AFGAtmosphereVolume::PostUnregisterAllComponents() {
	Super::PostUnregisterAllComponents();
	if (UWorld* World = GetWorld()) {
		World->RemovePostProcessVolume(this);
		RemoveVolume();
	}
}

bool AFGAtmosphereVolume::EncompassesPoint(FVector point, float sphereRadius, float* out_distanceToPoint) {
	return Super::EncompassesPoint(point, sphereRadius, out_distanceToPoint);
}

#define COPY_POST_PROCESSING_SETTING(AttribName) \
	const auto AttribName = FBiomeHelpers::Get##AttribName(mBiome, TimeOfDay); \
	if (AttribName.IsSet()) { \
		mPostProcessSettings.AttribName = AttribName.GetValue(); \
	}

FPostProcessVolumeProperties AFGAtmosphereVolume::GetProperties() const {
	AFGTimeOfDaySubsystem* TimeOfDaySubsystem = AFGTimeOfDaySubsystem::Get(GetWorld());

	mPostProcessSettings = FPostProcessSettings();
	if (TimeOfDaySubsystem != NULL && mBiome != NULL) {
		const float TimeOfDay = TimeOfDaySubsystem->GetTimeOfDayHours();

		COPY_POST_PROCESSING_SETTING(WhiteTemp);
		COPY_POST_PROCESSING_SETTING(WhiteTint);
		COPY_POST_PROCESSING_SETTING(ColorSaturation);
		COPY_POST_PROCESSING_SETTING(ColorContrast);
		COPY_POST_PROCESSING_SETTING(ColorGamma);
		COPY_POST_PROCESSING_SETTING(ColorGain);
		COPY_POST_PROCESSING_SETTING(ColorOffset);
		COPY_POST_PROCESSING_SETTING(ColorSaturationShadows);
		COPY_POST_PROCESSING_SETTING(ColorContrastShadows);
		COPY_POST_PROCESSING_SETTING(ColorGammaShadows);
		COPY_POST_PROCESSING_SETTING(ColorGainShadows);
		COPY_POST_PROCESSING_SETTING(ColorOffsetShadows);
		COPY_POST_PROCESSING_SETTING(ColorSaturationMidtones);
		COPY_POST_PROCESSING_SETTING(ColorContrastMidtones);
		COPY_POST_PROCESSING_SETTING(ColorGammaMidtones);
		COPY_POST_PROCESSING_SETTING(ColorGainMidtones);
		COPY_POST_PROCESSING_SETTING(ColorOffsetMidtones);
		COPY_POST_PROCESSING_SETTING(ColorSaturationHighlights);
		COPY_POST_PROCESSING_SETTING(ColorContrastHighlights);
		COPY_POST_PROCESSING_SETTING(ColorGammaHighlights);
		COPY_POST_PROCESSING_SETTING(ColorGainHighlights);
		COPY_POST_PROCESSING_SETTING(ColorOffsetHighlights);
		COPY_POST_PROCESSING_SETTING(ColorCorrectionHighlightsMin);
		COPY_POST_PROCESSING_SETTING(ColorCorrectionShadowsMax);
		COPY_POST_PROCESSING_SETTING(BlueCorrection);
		COPY_POST_PROCESSING_SETTING(ExpandGamut);
		COPY_POST_PROCESSING_SETTING(SceneColorTint);
		COPY_POST_PROCESSING_SETTING(FilmSlope);
		COPY_POST_PROCESSING_SETTING(FilmToe);
		COPY_POST_PROCESSING_SETTING(FilmShoulder);
		COPY_POST_PROCESSING_SETTING(FilmBlackClip);
		COPY_POST_PROCESSING_SETTING(FilmWhiteClip);
	}
	
	FPostProcessVolumeProperties Result{};
	Result.Settings = &mPostProcessSettings;
	Result.Priority = GetPriority();
	Result.BlendWeight = mBiome ? mBiome->mBlendWeight : 0.0f;
	Result.BlendRadius = GetBlendDistance();
	Result.bIsEnabled = IsHidden();
	Result.bIsUnbound = false;
	return Result;
}

void AFGAtmosphereVolume::GetSettings(FExponentialFogSettings& out_settings) const {
	const AFGTimeOfDaySubsystem* TimeOfDaySubsystem = AFGTimeOfDaySubsystem::Get(GetWorld());
	const float atTime = TimeOfDaySubsystem ? TimeOfDaySubsystem->GetTimeOfDayHours() : 0.0f;
	
	FBiomeHelpers::GetExponentialFogSettings(mBiome, atTime, out_settings);
}

void AFGAtmosphereVolume::GetSettings(float atTime, FExponentialFogSettings& out_settings) const {
	FBiomeHelpers::GetExponentialFogSettings(mBiome, atTime, out_settings);
}

void AFGAtmosphereVolume::GetSkySphereSettings(float atTime, FSkySphereSettings& out_settings) const {
	FBiomeHelpers::GetSkySphereSettings(mBiome, atTime, out_settings);
}

void AFGAtmosphereVolume::GetAtmosphereSettings(float atTime, FSkyAtmosphereSettings& out_settings) const {
	FBiomeHelpers::GetAtmosphereSettings(mBiome, atTime, out_settings);
}

void AFGAtmosphereVolume::OnConstruction(const FTransform& Transform) {
	Super::OnConstruction(Transform);
}

void AFGAtmosphereVolume::AddVolume() {
	if (AFGWorldSettings* WorldSettings = Cast<AFGWorldSettings>(GetWorld()->GetWorldSettings())) {
		WorldSettings->mFogVolumes.Add(this);
	}
}

void AFGAtmosphereVolume::RemoveVolume() {
	if (AFGWorldSettings* WorldSettings = Cast<AFGWorldSettings>(GetWorld()->GetWorldSettings())) {
		WorldSettings->mFogVolumes.Remove(this);
	}
}
