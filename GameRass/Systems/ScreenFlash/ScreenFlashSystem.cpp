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

#include <Events/SceneChange.h>
#include <Systems/GlobalEvents/IGlobalEventsSystem.h>
#include <Systems/Logging/ILoggingSystem.h>
#include <Systems/Scene/ISceneSystem.h>
#include <Utils.h>

using namespace RassEngine::Systems;
using namespace RassEngine::Events;

namespace RassGame::Systems {
ScreenFlashSystem::ScreenFlashSystem() :
	onSceneLoaded{this, &ScreenFlashSystem::OnSceneLoaded}
	, onSceneUnloaded{this, &ScreenFlashSystem::OnSceneUnloaded} {}

ScreenFlashSystem::~ScreenFlashSystem() {}

bool ScreenFlashSystem::Initialize() {
	// Make sure the scene system is available
	if(!ISceneSystem::Get()) {
		LOG_ERROR("{} failed to initialize: no scene system", NameClass());
		return false;
	}

	// Make sure the events system is available
	if(!IGlobalEventsSystem::Get()) {
		LOG_ERROR("{} failed to initialize: no global events system", NameClass());
		return false;
	}

	// Bind to scene load/unload events
	IGlobalEventsSystem::Get()->bind(SceneChange::AfterInitialize, &onSceneLoaded);
	IGlobalEventsSystem::Get()->bind(SceneChange::BeforeShutdown, &onSceneUnloaded);
	return true;
}

void ScreenFlashSystem::Shutdown() {
	if(!IGlobalEventsSystem::Get()) {
		LOG_ERROR("{} failed to initialize: no global events system", NameClass());
		return;
	}

	// Unbind to scene load/unload events
	IGlobalEventsSystem::Get()->unbind(SceneChange::AfterInitialize, &onSceneLoaded);
	IGlobalEventsSystem::Get()->unbind(SceneChange::BeforeShutdown, &onSceneUnloaded);
}

bool ScreenFlashSystem::Show(const glm::vec3 &color, const RassEngine::TweenCurve &curve) {
	return flashComponent ? flashComponent->Show(color, curve) : false;
}

bool ScreenFlashSystem::OnSceneLoaded(const RassEngine::IEvent<RassEngine::Events::GlobalEventArgs> *, const RassEngine::Events::GlobalEventArgs &) {
	// FIXME: load the screen flasher
	// FIXME: set the screenflash pointer
	return true;
}

bool ScreenFlashSystem::OnSceneUnloaded(const RassEngine::IEvent<RassEngine::Events::GlobalEventArgs> *, const RassEngine::Events::GlobalEventArgs &) {
	// Revert the screenflasher to null
	flashComponent = nullptr;
	return true;
}

const std::string_view &ScreenFlashSystem::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassGame::Systems::ScreenFlashSystem);
	return className;
}
}
