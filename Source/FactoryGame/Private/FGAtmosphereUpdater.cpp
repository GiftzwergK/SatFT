#include "FGAtmosphereUpdater.h"
#include "FGTimeSubsystem.h"
#include "FGWorldSettings.h"
#include "Atmosphere/BiomeHelpers.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Engine/ExponentialHeightFog.h"

void RegisterAtmosphereUpdater() {
	static FDelegateHandle PostEditorTickDelegate = FDelegateHandle();
	
	FCoreDelegates::OnPostEngineInit.AddLambda([]() {
		UFGAtmosphereUpdater* AtmosphereUpdater = NewObject<UFGAtmosphereUpdater>(GEngine, TEXT("FGAtmosphereUpdater"));
		AtmosphereUpdater->AddToRoot();
		UFGAtmosphereUpdater::AtmosphereUpdaterStatic = AtmosphereUpdater;
		PostEditorTickDelegate = GEngine->OnPostEditorTick().AddUObject(AtmosphereUpdater, &UFGAtmosphereUpdater::Tick);
	});
	
	FCoreDelegates::OnEnginePreExit.AddLambda([]() {
		GEngine->OnPostEditorTick().Remove(PostEditorTickDelegate);
		UFGAtmosphereUpdater::AtmosphereUpdaterStatic->RemoveFromRoot();
		UFGAtmosphereUpdater::AtmosphereUpdaterStatic = NULL;
	});
}

UFGAtmosphereUpdater::UFGAtmosphereUpdater() {
	this->mDirectionalLightMultiplierOverride = 1.0f;
	this->mSkylightLightMultiplierOverride = 1.0f;
	this->mDebuggingEnabled = false;
}

void UFGAtmosphereUpdater::Tick(float dt) {
	const UEngine* Engine = Cast<UEngine>(GetOuter());
	if (Engine != NULL) {
		for (const FWorldContext& WorldContext : Engine->GetWorldContexts()) {
			if (ShouldUpdateWorld(WorldContext.World())) {
				UpdateWorld(WorldContext.World());
			}
		}
	}
}

void UFGAtmosphereUpdater::FinishDestroy() {
	AtmosphereUpdaterStatic = NULL;
	Super::FinishDestroy();
}

void UFGAtmosphereUpdater::ApplyFogSettings(const FExponentialFogSettings& fogSettings,  UWorld* world) const {
	if (const AFGWorldSettings* FGWorldSettings = Cast<AFGWorldSettings>(world->GetWorldSettings())) {
		if (AExponentialHeightFog* ExponentialHeightFog = FGWorldSettings->GetExponentialHeightFog()) {
			UExponentialHeightFogComponent* HeightFogComponent = ExponentialHeightFog->GetComponent();

			HeightFogComponent->SetFogDensity(fogSettings.FogDensity);
			HeightFogComponent->SetFogInscatteringColor(fogSettings.FogInscatteringColor);
			HeightFogComponent->SetFullyDirectionalInscatteringColorDistance(fogSettings.FullyDirectionalInscatteringColorDistance);
			HeightFogComponent->SetNonDirectionalInscatteringColorDistance(fogSettings.NonDirectionalInscatteringColorDistance);
			HeightFogComponent->SetDirectionalInscatteringExponent(fogSettings.DirectionalInscatteringExponent);
			HeightFogComponent->SetDirectionalInscatteringStartDistance(fogSettings.DirectionalInscatteringStartDistance);
			HeightFogComponent->SetDirectionalInscatteringColor(fogSettings.DirectionalInscatteringColor);
			HeightFogComponent->SetFogHeightFalloff(fogSettings.FogHeightFalloff);
			HeightFogComponent->SetFogMaxOpacity(fogSettings.FogMaxOpacity);
			HeightFogComponent->SetStartDistance(fogSettings.StartDistance);
			HeightFogComponent->SetFogCutoffDistance(fogSettings.FogCutoffDistance);
			HeightFogComponent->SecondFogData.FogDensity = fogSettings.SecondFogDensity;
			HeightFogComponent->SecondFogData.FogHeightFalloff = fogSettings.SecondFogHeightFalloff;
			HeightFogComponent->SecondFogData.FogHeightOffset = fogSettings.SecondFogHeightOffset;

			FVector NewFogLocation = ExponentialHeightFog->GetActorLocation();
			NewFogLocation.Z = fogSettings.FogHeight;
			ExponentialHeightFog->SetActorLocation(NewFogLocation);

			HeightFogComponent->SetFogInscatteringColor(fogSettings.FogInscatteringColor);
			HeightFogComponent->SetDirectionalInscatteringColor(fogSettings.DirectionalInscatteringColor);
		}
	}
}

void UFGAtmosphereUpdater::ApplySkyAtmosphereSettings(const FSkyAtmosphereSettings& settings,  UWorld* world) const {
	if (const AFGWorldSettings* FGWorldSettings = Cast<AFGWorldSettings>(world->GetWorldSettings())) {
		if (const ASkyAtmosphere* SkyAtmosphere = FGWorldSettings->GetSkyAtmosphere()) {
			USkyAtmosphereComponent* SkyAtmosphereComponent = SkyAtmosphere->GetComponent();

			SkyAtmosphereComponent->SetRayleighScattering(settings.RayleighScattering);
			SkyAtmosphereComponent->SetRayleighScatteringScale(settings.RayleighScatteringScale);
			SkyAtmosphereComponent->SetRayleighExponentialDistribution(settings.RayleighExponentialDistribution);

			SkyAtmosphereComponent->SetMieScatteringScale(settings.MieScatteringScale);
			SkyAtmosphereComponent->SetMieScattering(settings.MieScattering);
			SkyAtmosphereComponent->SetMieAbsorptionScale(settings.MieAbsorptionScale);
			SkyAtmosphereComponent->SetMieAbsorption(settings.MieAbsorption);
			SkyAtmosphereComponent->SetMieAnisotropy(settings.MieAnisotropy);
			SkyAtmosphereComponent->SetMieExponentialDistribution(settings.MieExponentialDistribution);
			SkyAtmosphereComponent->SetOtherAbsorptionScale(settings.OtherAbsorptionScale);
			SkyAtmosphereComponent->SetOtherAbsorption(settings.OtherAbsorption);
		}
	}
}

void UFGAtmosphereUpdater::InterpolateFogSettings(FExponentialFogSettings& dest, const FExponentialFogSettings& src, float alpha) const {
	if (src.EnableFogDensity) {
		dest.FogDensity = FMath::Lerp(dest.FogDensity, src.FogDensity, alpha);
	}
	if (src.EnableFogInscatteringColor) {
		dest.FogInscatteringColor = FMath::Lerp(dest.FogInscatteringColor, src.FogInscatteringColor, alpha);
	}
	if (src.EnableFullyDirectionalInscatteringColorDistance) {
		dest.FullyDirectionalInscatteringColorDistance = FMath::Lerp(dest.FullyDirectionalInscatteringColorDistance, src.FullyDirectionalInscatteringColorDistance, alpha);
	}
	if (src.EnableNonDirectionalInscatteringColorDistance) {
		dest.NonDirectionalInscatteringColorDistance = FMath::Lerp(dest.NonDirectionalInscatteringColorDistance, src.NonDirectionalInscatteringColorDistance, alpha);
	}
	if (src.EnableDirectionalInscatteringExponent) {
		dest.DirectionalInscatteringExponent = FMath::Lerp(dest.DirectionalInscatteringExponent, src.DirectionalInscatteringExponent, alpha);
	}
	if (src.EnableDirectionalInscatteringStartDistance) {
		dest.DirectionalInscatteringStartDistance = FMath::Lerp(dest.DirectionalInscatteringStartDistance, src.DirectionalInscatteringStartDistance, alpha);
	}
	if (src.EnableDirectionalInscatteringColor) {
		dest.DirectionalInscatteringColor = FMath::Lerp(dest.DirectionalInscatteringColor, src.DirectionalInscatteringColor, alpha);
	}
	if (src.EnableFogHeightFalloff) {
		dest.FogHeightFalloff = FMath::Lerp(dest.FogHeightFalloff, src.FogHeightFalloff, alpha);
	}
	if (src.EnableFogMaxOpacity) {
		dest.FogMaxOpacity = FMath::Lerp(dest.FogMaxOpacity, src.FogMaxOpacity, alpha);
	}
	if (src.EnableStartDistance) {
		dest.StartDistance = FMath::Lerp(dest.StartDistance, src.StartDistance, alpha);
	}
	if (src.EnableFogCutoffDistance) {
		dest.FogCutoffDistance = FMath::Lerp(dest.FogCutoffDistance, src.FogCutoffDistance, alpha);
	}
	if (src.EnableFogHeight) {
		dest.FogHeight = FMath::Lerp(dest.FogHeight, src.FogHeight, alpha);
	}
	if (src.EnableSecondFogDensity) {
		dest.SecondFogDensity = FMath::Lerp(dest.SecondFogDensity, src.SecondFogDensity, alpha);
	}
	if (src.EnableSecondFogHeightFalloff) {
		dest.SecondFogHeightFalloff = FMath::Lerp(dest.SecondFogHeightFalloff, src.SecondFogHeightFalloff, alpha);
	}
	if (src.EnableSecondFogHeightOffset) {
		dest.SecondFogHeightOffset = FMath::Lerp(dest.SecondFogHeightOffset, src.SecondFogHeightFalloff, alpha);
	}
}

void UFGAtmosphereUpdater::InterpolateSkySphereSettings(FSkySphereSettings& dest, const FSkySphereSettings& src, float alpha) const {
	if (src.OverrideSunLightColor) {
		dest.SunLightColor = FMath::Lerp(dest.SunLightColor, src.SunLightColor, alpha);
	}
	if (src.OverrideMoonLightColor) {
		dest.MoonLightColor = FMath::Lerp(dest.MoonLightColor, src.MoonLightColor, alpha);
	}
	if (src.OverrideSunIntensity) {
		dest.SunIntensity = FMath::Lerp(dest.SunIntensity, src.SunIntensity, alpha);
	}
	if (src.OverrideMoonIntensity) {
		dest.MoonIntensity = FMath::Lerp(dest.MoonIntensity, src.MoonIntensity, alpha);
	}
}

void UFGAtmosphereUpdater::InterpolateAtmosphereSettings(FSkyAtmosphereSettings& dest, const FSkyAtmosphereSettings& src, float alpha) const {
	if (src.OverrideRayleighScattering) {
		dest.RayleighScattering = FMath::Lerp(dest.RayleighScattering, src.RayleighScattering, alpha);
	}
	if (src.OverrideRayleighScatteringScale) {
		dest.RayleighScatteringScale = FMath::Lerp(dest.RayleighScatteringScale, src.RayleighScatteringScale, alpha);
	}
	if(src.OverrideRayleighExponentialDistribution) {
		dest.RayleighExponentialDistribution = FMath::Lerp(dest.RayleighExponentialDistribution, src.RayleighExponentialDistribution, alpha);
	}
	if (src.OverrideMieScatteringScale) {
		dest.MieScatteringScale = FMath::Lerp(dest.MieScatteringScale, src.MieScatteringScale, alpha);
	}
	if (src.OverrideMieScattering) {
		dest.MieScattering = FMath::Lerp(dest.MieScattering, src.MieScattering, alpha);
	}
	if (src.OverrideMieAnisotropy) {
		dest.MieAnisotropy = FMath::Lerp(dest.MieAnisotropy, src.MieAnisotropy, alpha);
	}
	if (src.OverrideMieExponentialDistribution) {
		dest.MieExponentialDistribution = FMath::Lerp(dest.MieExponentialDistribution, src.MieExponentialDistribution, alpha);
	}
	if (src.OverrideOtherAbsorptionScale) {
		dest.OtherAbsorptionScale = FMath::Lerp(dest.OtherAbsorptionScale, src.OtherAbsorptionScale, alpha);
	}
	if (src.OverrideOtherAbsorption) {
		dest.OtherAbsorption = FMath::Lerp(dest.OtherAbsorption, src.OtherAbsorption, alpha);
	}
}

void UFGAtmosphereUpdater::UpdateWorld(UWorld* world) {
	AFGWorldSettings* WorldSettings = Cast<AFGWorldSettings>(world->GetWorldSettings());
	if (WorldSettings == NULL) {
		mBlendingVolumes.Empty();
		return;
	}
	AFGTimeOfDaySubsystem* TimeOfDaySubsystem = AFGTimeOfDaySubsystem::Get(world);
	if (TimeOfDaySubsystem == NULL) {
		mBlendingVolumes.Empty();
		return;
	}
	UEngine* Engine = CastChecked<UEngine>(GetOuter());
	const ULocalPlayer* LocalPlayer = Engine->GetGamePlayer(world, 0);
	if (LocalPlayer == NULL) {
		mBlendingVolumes.Empty();
		return;
	}
	const FVector LastViewLocation = LocalPlayer->LastViewLocation;
	
	mBlendingVolumes.Empty();
	for (AFGAtmosphereVolume* AtmosphereVolume : WorldSettings->mFogVolumes) {
		if (!AtmosphereVolume->IsHidden() && AtmosphereVolume->mBiome) {
			const float SphereRadius = AtmosphereVolume->GetBounds().SphereRadius;
			float OutAtmosphereVolumeDistance = 0.0f;
			AtmosphereVolume->EncompassesPoint(LastViewLocation, SphereRadius, &OutAtmosphereVolumeDistance);

			if (OutAtmosphereVolumeDistance < AtmosphereVolume->GetBlendDistance()) {
				if (OutAtmosphereVolumeDistance > SMALL_NUMBER) {
					FAtmosphereVolumeBlend VolumeBlend;
					VolumeBlend.Volume = AtmosphereVolume;
					VolumeBlend.BlendRatio = 1.0f - OutAtmosphereVolumeDistance / AtmosphereVolume->GetBlendDistance();
					mBlendingVolumes.Add(VolumeBlend);
				} else {
					FAtmosphereVolumeBlend VolumeBlend;
					VolumeBlend.Volume = AtmosphereVolume;
					VolumeBlend.BlendRatio = 1.0f;
					mBlendingVolumes.Add(VolumeBlend);
				}
			}
		}
	}

	//TODO: This piece of code is different from the disassembly, but I cba to reverse engineer weird blending ratio calculations with nested loops
	mBlendingVolumes.StableSort([&](const FAtmosphereVolumeBlend& A, const FAtmosphereVolumeBlend& B) {
		const float PriorityA = A.Volume->GetPriority();
		const float PriorityB = B.Volume->GetPriority();

		//First we blend in volumes with the lowest priority (like a global one that always has 100% blend ratio)
		if (FMath::Abs(PriorityA - PriorityB) > SMALL_NUMBER) {
			return PriorityA < PriorityB;
		}
		//If the priority is the same, we blend higher ratio first, so the lower one can get at least some of the influence
		return A.BlendRatio > B.BlendRatio;
	});

	//Apply blending shenaningans
	const float TimeOfDayHours = TimeOfDaySubsystem->GetTimeOfDayHours();
	
	FExponentialFogSettings ResultFogSettings = WorldSettings->mDefaultHeightFogSettings;
	FSkySphereSettings ResultSkySphereSettings{};
	FSkyAtmosphereSettings ResultAtmosphereSettings{};

	if (AFGSkySphere* SkySphere = WorldSettings->GetSkySphere()) {
		SkySphere->GetSkySphereSettings(TimeOfDayHours, ResultSkySphereSettings);
	}

	for (const FAtmosphereVolumeBlend& VolumeBlend : mBlendingVolumes) {
		FExponentialFogSettings BlendExponentialFogSettings{};
		FSkySphereSettings BlendSkySphereSettings{};
		FSkyAtmosphereSettings BlendSkyAtmosphereSettings{};

		const AFGAtmosphereVolume* AtmosphereVolume = VolumeBlend.Volume.Get();
		FBiomeHelpers::GetExponentialFogSettings(AtmosphereVolume->mBiome, TimeOfDayHours, BlendExponentialFogSettings);
		FBiomeHelpers::GetSkySphereSettings(AtmosphereVolume->mBiome, TimeOfDayHours, BlendSkySphereSettings);
		FBiomeHelpers::GetAtmosphereSettings(AtmosphereVolume->mBiome, TimeOfDayHours, BlendSkyAtmosphereSettings);

		InterpolateFogSettings(ResultFogSettings, BlendExponentialFogSettings, VolumeBlend.BlendRatio);
		InterpolateSkySphereSettings(ResultSkySphereSettings, BlendSkySphereSettings, VolumeBlend.BlendRatio);
		InterpolateAtmosphereSettings(ResultAtmosphereSettings, BlendSkyAtmosphereSettings, VolumeBlend.BlendRatio);
	}

	ResultSkySphereSettings.MoonIntensity *= mDirectionalLightMultiplierOverride;
	ResultSkySphereSettings.SunIntensity *= mDirectionalLightMultiplierOverride;

	ApplyFogSettings(ResultFogSettings, world);
	if (AFGSkySphere* SkySphere = WorldSettings->GetSkySphere()) {
		SkySphere->ApplySkySphereSettings(ResultSkySphereSettings);
	}
	ApplySkyAtmosphereSettings(ResultAtmosphereSettings, world);
}

bool UFGAtmosphereUpdater::ShouldUpdateWorld(UWorld* world) const {
	return !world->IsPendingKill() && (
		world->WorldType == EWorldType::Game ||
		world->WorldType == EWorldType::Editor ||
		world->WorldType == EWorldType::PIE);
}

void UFGAtmosphereUpdater::AtmosphereDebuggingChanged(IConsoleVariable *var) {
}
