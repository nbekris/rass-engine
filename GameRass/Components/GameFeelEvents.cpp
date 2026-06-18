// File Name:    GameFeelEvents.cpp
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Component controlling GameFeelEvents.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "pch.h"
#include "GameFeelEvents.h"

#include <Cloneable.h>
#include <Component.h>
#include <Components/Collider.h>
#include <Entity.h>
#include <IEvent.h>
#include <Stream.h>
#include <string_view>
#include <Systems/Logging/ILoggingSystem.h>
#include <Systems/Scene/ISceneSystem.h>
#include <Utils.h>
#include <UUID.h>

#include "Components/SpriteSwapper.h"
#include "Components/Switch.h"

using namespace RassEngine;
using namespace RassEngine::Components;
using namespace RassEngine::Events;
using namespace RassEngine::Systems;

namespace RassGame::Components {

GameFeelEvents::GameFeelEvents() : Cloneable<Component, GameFeelEvents>{}
{
	eventSettings.reserve(3);
}

GameFeelEvents::GameFeelEvents(const GameFeelEvents &other) : Cloneable<Component, GameFeelEvents>{other}
	, eventSettings{other.eventSettings}
{}

bool GameFeelEvents::Initialize() {
	return true;
}

GameFeelEvents::~GameFeelEvents() {
}

const std::string_view &GameFeelEvents::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Components::GameFeelEvents);
	return className;
}

bool GameFeelEvents::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}

	eventSettings.clear();
	stream.ReadObject("Events", [this, &stream] (const std::string &eventName) {
		Settings settings;
		if(!settings.Read(stream)) {
			LOG_WARNING("{}: Failed to read settings for event '{}'", NameClass(), eventName);
			return;
		}
		eventSettings.emplace(std::move(eventName), std::move(settings));
	});
	return true;
}

bool 	GameFeelEvents::Settings::Play() const {
	// FIXME: to implement
	return true;
}

bool GameFeelEvents::Settings::Read(RassEngine::Stream &stream) {
	// FIXME: to implement
	return true;
}

}
