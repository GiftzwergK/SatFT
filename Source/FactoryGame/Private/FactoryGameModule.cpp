#include "FactoryGameModule.h"

extern void RegisterAtmosphereUpdater();

void FFactoryGameModule::StartupModule() {
	RegisterAtmosphereUpdater();
}

void FFactoryGameModule::ShutdownModule() {
}

IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, FactoryGame, "FactoryGame");
