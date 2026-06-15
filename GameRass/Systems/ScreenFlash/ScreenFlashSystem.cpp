// File Name:    ScreenFlashSystem.cpp
// Author(s):    main Steven Yacoub, secondary Taro Omiya, Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      System providing custom memory allocation.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include <Precompiled.h>
#include "ScreenFlashSystem.h"

#include <string_view>

#include <Systems/Logging/ILoggingSystem.h>
#include <Utils.h>

namespace RassGame::Systems {
ScreenFlashSystem::ScreenFlashSystem() :
	onSceneLoaded{this, &ScreenFlashSystem::OnSceneLoaded}
	, onSceneUnloaded{this, &ScreenFlashSystem::OnSceneUnloaded} {}

ScreenFlashSystem::~ScreenFlashSystem() {}

bool ScreenFlashSystem::Initialize() {
	// FIXME: bind to scene load/unload events
	return true;
}

void ScreenFlashSystem::Shutdown() {
	// FIXME: unbind to scene load/unload events
}

bool ScreenFlashSystem::Show(const glm::vec3 &color, const RassEngine::TweenCurve &curve) {
	return flashComponent ? flashComponent->Show(color, curve) : false;
}

bool ScreenFlashSystem::OnSceneLoaded(const RassEngine::IEvent<RassEngine::Events::GlobalEventArgs> *, const RassEngine::Events::GlobalEventArgs &) {
	return false;
}

bool ScreenFlashSystem::OnSceneUnloaded(const RassEngine::IEvent<RassEngine::Events::GlobalEventArgs> *, const RassEngine::Events::GlobalEventArgs &) {
	return false;
}

const std::string_view &ScreenFlashSystem::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassGame::Systems::ScreenFlashSystem);
	return className;
}
}
