#include <Precompiled.h>
#include "GameFeelActionFactory.h"
#include "ScreenShakeAction.h"
#include "ScreenFlashAction.h"
#include "HitStopAction.h"
#include "SpawnParticlesAction.h"

namespace RassGame::Systems {
std::unordered_map<std::string, GameFeelActionFactory::Constructor> &GameFeelActionFactory::Registry() {
	static std::unordered_map<std::string, Constructor> registry;
	return registry;
}
void GameFeelActionFactory::EnsureDefaults() {
	static bool initialized = false;
	if(initialized) {
		return;
	}
	initialized = true;
	Register("ScreenShake", [] { return std::make_shared<ScreenShakeAction>(); });
	Register("ScreenFlash", [] { return std::make_shared<ScreenFlashAction>(); });
	Register("HitStop", [] { return std::make_shared<HitStopAction>(); });
	Register("SpawnParticles", [] { return std::make_shared<SpawnParticlesAction>(); });
}
bool GameFeelActionFactory::Register(const std::string &type, Constructor constructor) {
	Registry()[type] = std::move(constructor);
	return true;
}
std::shared_ptr<IGameFeelAction> GameFeelActionFactory::Create(const std::string &type) {
	EnsureDefaults();
	auto it = Registry().find(type);
	return it != Registry().end() ? it->second() : nullptr;
}
}
