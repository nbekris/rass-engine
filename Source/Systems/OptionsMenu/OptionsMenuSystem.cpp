// File Name:    OptionsMenuSystem.cpp
// Author(s):    main Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      System managing the options menu UI.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "OptionsMenuSystem.h"

#include "Events/Global.h"
#include "Events/GlobalEventArgs.h"
#include "IEvent.h"
#include "Scene.h"
#include "Systems/Audio/AudioOptionsUI.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Systems/Scene/ISceneSystem.h"
#include "Utils.h"

namespace RassEngine::Systems {

OptionsMenuSystem::OptionsMenuSystem()
	: renderListener{this, &OptionsMenuSystem::Render}
{}

OptionsMenuSystem::~OptionsMenuSystem() {
	Shutdown();
}

bool OptionsMenuSystem::Initialize() {
	if(IGlobalEventsSystem::Get() == nullptr) {
		LOG_ERROR("Cannot setup options menu: unable to bind to {}", NAMEOF(Systems::IGlobalEventsSystem));
		return false;
	}

	IGlobalEventsSystem::Get()->bind(Events::Global::Render, &renderListener);
	return true;
}

void OptionsMenuSystem::Shutdown() {
	if(IGlobalEventsSystem::Get() == nullptr) {
		return;
	}

	IGlobalEventsSystem::Get()->unbind(Events::Global::Render, &renderListener);
}

const std::string_view &OptionsMenuSystem::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Systems::OptionsMenuSystem);
	return className;
}

bool OptionsMenuSystem::Render(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
	auto *sceneSystem = ISceneSystem::Get();
	if(!sceneSystem) return true;

	Scene *scene = sceneSystem->GetCurrentScene();
	if(!scene) return true;

	if(scene->Name() == "OptionsMenu") {
		RenderAudioSliders();
	}

	return true;
}

}
