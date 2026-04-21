// File Name:    TimedSceneTransition.cpp
// Author(s):    main Niko Bekris, secondary Steven Yacoub
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that transitions to a new scene after a delay.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "pch.h"
#include "TimedSceneTransition.h"

#include <Cloneable.h>
#include <Component.h>
#include <Entity.h>
#include <Events/GlobalEventArgs.h>
#include <Events/Global.h>
#include <Events/GlobalEventListener.h>
#include <Events/TriggerEventArgs.h>
#include <Events/TriggerEventIDs.h>
#include <Scenes/SerializedScene.h>
#include <Stream.h>
#include <string_view>
#include <Systems/Logging/ILoggingSystem.h>
#include <Systems/Time/ITimeSystem.h>
#include <Systems/Scene/ISceneSystem.h>
#include <Systems/Input/IInputSystem.h>
#include <Utils.h>
#include <Systems/GlobalEvents/IGlobalEventsSystem.h>
#include <GLFW/glfw3.h>

using namespace RassEngine;
using namespace RassEngine::Events;
using namespace RassEngine::Components;
using namespace RassEngine::Scenes;
using namespace RassEngine::Systems;

namespace RassGame::Components {
static constexpr std::string_view KEY_NEXT_SCENE = "NextScene";
static constexpr std::string_view KEY_BYPASS_SCENE_TO = "SceneToBypassTo";
static constexpr std::string_view KEY_SCENE_DURATION = "SceneDuration";

TimedSceneTransition::TimedSceneTransition() : Cloneable<Component, TimedSceneTransition>{},
onUpdateListener{this, &TimedSceneTransition::OnUpdate}
{ }

TimedSceneTransition::TimedSceneTransition(const TimedSceneTransition &other) : Cloneable<Component, TimedSceneTransition>{other}
, nextScene{other.nextScene}, sceneToBypassTo{other.sceneToBypassTo}, onUpdateListener{this, &TimedSceneTransition::OnUpdate} { }

bool TimedSceneTransition::Initialize() {
	if(Parent() == nullptr) {
		LOG_WARNING("{}: Entity was not found", NameClass());
		return false;
	}

	if(ISceneSystem::Get() == nullptr) {
		LOG_WARNING("{}: {} is not registered", NameClass(), NAMEOF(ISceneSystem));
		return false;
	}

	if(IGlobalEventsSystem::Get() != nullptr) {
		IGlobalEventsSystem::Get()->bind(Events::Global::Update, &onUpdateListener);
	}

	if(IInputSystem::Get() != nullptr) {
		inputSystem = IInputSystem::Get();
	}

	return true;
}

TimedSceneTransition::~TimedSceneTransition() {
	if(IGlobalEventsSystem::Get() != nullptr) {
		IGlobalEventsSystem::Get()->unbind(Events::Global::Update, &onUpdateListener);
	}

	if(Parent() == nullptr) {
		LOG_WARNING("{}: Entity was not found", NameClass());
		return;
	}
}

const std::string_view &TimedSceneTransition::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Components::TimedSceneTransition);
	return className;
}

bool TimedSceneTransition::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}

	stream.Read(KEY_NEXT_SCENE, nextScene);
	stream.Read(KEY_SCENE_DURATION, sceneDuration);
	stream.Read(KEY_BYPASS_SCENE_TO, sceneToBypassTo);
	return true;
}

bool TimedSceneTransition::OnUpdate(const IEvent<GlobalEventArgs>*, const GlobalEventArgs&) {
	float dt = ITimeSystem::Get()->GetDeltaTimeSec();
	

	bool bypassCheck = false;

	if(inputSystem->IsKeyPressed(GLFW_KEY_ENTER) || inputSystem->IsKeyPressed(GLFW_KEY_SPACE)
		|| inputSystem->IsKeyPressed(GLFW_KEY_ESCAPE) || inputSystem->IsMouseButtonClicked(GLFW_MOUSE_BUTTON_LEFT)
		|| inputSystem->IsMouseButtonClicked(GLFW_MOUSE_BUTTON_RIGHT)) {
		bypassCheck = true;
	}

	if(bypassCheck) {
		bypassTransition(sceneToBypassTo);
		return true;
	}

	currentTime += dt;
	if(currentTime >= sceneDuration) {
		bypassTransition(nextScene);
	}
	return true;
}

void TimedSceneTransition::bypassTransition(const std::string &sceneName) {
	if(!sceneName.empty()) {
		ISceneSystem::Get()->SetPendingScene<SerializedScene>(sceneName);
		this->sceneDuration = std::numeric_limits<float>::max();
	}
}

}
