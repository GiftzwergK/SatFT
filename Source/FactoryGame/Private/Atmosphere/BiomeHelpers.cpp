#include "Atmosphere/BiomeHelpers.h"
#include "FGAtmosphereVolume.h"

#define COPY_BIOME_SETTING(AttribName) \
	const auto AttribName = FBiomeHelpers::Get##AttribName(biome, atTime); \
	if (AttribName.IsSet()) { \
		out_settings.AttribName = AttribName.GetValue(); \
	}


void FBiomeHelpers::GetExponentialFogSettings(UFGBiome *biome, float atTime, FExponentialFogSettings& out_settings) {
	COPY_BIOME_SETTING(FogHeight);
	COPY_BIOME_SETTING(FogDensity);
	COPY_BIOME_SETTING(FogInscatteringColor);
	COPY_BIOME_SETTING(FullyDirectionalInscatteringColorDistance);
	COPY_BIOME_SETTING(NonDirectionalInscatteringColorDistance);
	COPY_BIOME_SETTING(DirectionalInscatteringExponent);
	COPY_BIOME_SETTING(DirectionalInscatteringStartDistance);
	COPY_BIOME_SETTING(DirectionalInscatteringColor);
	COPY_BIOME_SETTING(FogHeightFalloff);
	COPY_BIOME_SETTING(FogMaxOpacity);
	COPY_BIOME_SETTING(StartDistance);
	COPY_BIOME_SETTING(FogCutoffDistance);
	COPY_BIOME_SETTING(SecondFogDensity);
	COPY_BIOME_SETTING(SecondFogHeightFalloff);
	COPY_BIOME_SETTING(SecondFogHeightOffset);
}

void FBiomeHelpers::GetSkySphereSettings(UFGBiome *biome, float atTime, FSkySphereSettings& out_settings) {
	COPY_BIOME_SETTING(SunLightColor);
	COPY_BIOME_SETTING(MoonLightColor);
	COPY_BIOME_SETTING(SunIntensity);
	COPY_BIOME_SETTING(MoonIntensity);
}

void FBiomeHelpers::GetAtmosphereSettings(UFGBiome *biome, float atTime, FSkyAtmosphereSettings& out_settings) {
	COPY_BIOME_SETTING(RayleighScattering);
	COPY_BIOME_SETTING(RayleighScatteringScale);
	COPY_BIOME_SETTING(RayleighExponentialDistribution);
	COPY_BIOME_SETTING(MieScatteringScale);
	COPY_BIOME_SETTING(MieScattering);
	COPY_BIOME_SETTING(MieAbsorptionScale);
	COPY_BIOME_SETTING(MieAbsorption);
	COPY_BIOME_SETTING(MieAnisotropy);
	COPY_BIOME_SETTING(MieExponentialDistribution);
	COPY_BIOME_SETTING(OtherAbsorptionScale);
	COPY_BIOME_SETTING(OtherAbsorption)
}
