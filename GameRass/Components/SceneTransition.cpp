// File Name:    SceneTransition.cpp
// Author(s):    main Niko Bekris, secondary Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Component managing animated transitions between scenes.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "pch.h"
#include "SceneTransition.h"

#include <Cloneable.h>
#include <Component.h>
#include <Entity.h>
#include <Events/EventArgs.h>
#include <Events/EventListenerMethod.h>
#include <Events/TriggerEventArgs.h>
#include <Events/TriggerEventIDs.h>
#include <Scenes/SerializedScene.h>
#include <Stream.h>
#include <string_view>
#include <Systems/Logging/ILoggingSystem.h>
#include <Systems/Scene/ISceneSystem.h>
#include <Utils.h>

using namespace RassEngine;
using namespace RassEngine::Events;
using namespace RassEngine::Components;
using namespace RassEngine::Scenes;
using namespace RassEngine::Systems;

namespace RassGame::Components {
static constexpr std::string_view KEY_NEXT_SCENE = "NextScene";
static constexpr std::string_view KEY_TRIGGERED_BY = "TriggeredBy";

SceneTransition::SceneTransition() : Cloneable<Component, SceneTransition>{}
, onTriggerEnter{this, &SceneTransition::OnTriggerEnter}
{ }

SceneTransition::SceneTransition(const SceneTransition &other) : Cloneable<Component, SceneTransition>{other}
, nextScene{other.nextScene}, triggeredBy{other.triggeredBy}, onTriggerEnter{this, &SceneTransition::OnTriggerEnter}
{ }

bool SceneTransition::Initialize() {
	if(Parent() == nullptr) {
		LOG_WARNING("{}: Entity was not found", NameClass());
		return false;
	}

	Parent()->BindEvent(Events::TriggerEventID::OnTriggerEnter, &onTriggerEnter);

	if(ISceneSystem::Get() == nullptr) {
		LOG_WARNING("{}: {} is not registered", NameClass(), NAMEOF(ISceneSystem));
		return false;
	}
	return true;
}

SceneTransition::~SceneTransition() {
	if(Parent() == nullptr) {
		LOG_WARNING("{}: Entity was not found", NameClass());
		return;
	}

	Parent()->UnbindEvent(Events::TriggerEventID::OnTriggerEnter, &onTriggerEnter);
}

const std::string_view &SceneTransition::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Components::SceneTransition);
	return className;
}

bool SceneTransition::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}

	stream.Read(KEY_TRIGGERED_BY, triggeredBy);
	stream.Read(KEY_NEXT_SCENE, nextScene);
	return true;
}

bool SceneTransition::OnTriggerEnter(const IEvent<EventArgs> *, const EventArgs &args) {
	if(!IsEnabled()) {
		return true;
	}

	// Check what caused the trigger
	const Entity *otherEntity = dynamic_cast<const TriggerEventArgs &>(args).otherEntity;
	if((otherEntity == nullptr) || !otherEntity->IsNamed(triggeredBy)) {
		return true;
	}

	if(ISceneSystem::Get() == nullptr) {
		LOG_WARNING("{}: {} is not registered", NameClass(), NAMEOF(ISceneSystem));
		return false;
	}

	ISceneSystem::Get()->SetPendingScene<SerializedScene>(nextScene);
	return true;
}

}
