// File Name:    GameFeelEvents.cpp
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Component controlling GameFeelEvents.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "pch.h"
#include "GameFeelEvents.h"

#include <memory>
#include <string>
#include <string_view>

#include <Cloneable.h>
#include <Component.h>
#include <Entity.h>
#include <Events/EventArgs.h>
#include <Stream.h>
#include <Systems/Logging/ILoggingSystem.h>
#include <Utils.h>

using namespace RassEngine;
using namespace RassEngine::Components;
using namespace RassEngine::Events;
using namespace RassEngine::Systems;

namespace RassGame::Components {

GameFeelEvents::GameFeelEvents()
	: Cloneable<Component, GameFeelEvents>{}
{}

GameFeelEvents::GameFeelEvents(const GameFeelEvents &other)
	: Cloneable<Component, GameFeelEvents>{other}
	, allActionLists{other.allActionLists}
{}

bool GameFeelEvents::Initialize() {
	if(Parent() == nullptr) {
		LOG_ERROR("{}: Parent {} not found", NameClass(), NAMEOF(Entity));
		return false;
	}

	// Bind everything
	for(const auto &event : allActionLists) {
		for(const auto &action : event.second.actions) {
			Parent()->BindEvent(event.second.id, action->GetListener());
		}
	}

	// Indicate initialization is done
	isInitialized = true;
	return true;
}

GameFeelEvents::~GameFeelEvents() {
	if(Parent() == nullptr) {
		LOG_ERROR("{}: Parent {} not found", NameClass(), NAMEOF(Entity));
		return;
	}

	isInitialized = false;

	// Unbind everything
	for(const auto &event : allActionLists) {
		for(const auto &action : event.second.actions) {
			Parent()->UnbindEvent(event.second.id, action->GetListener());
		}
	}

	// Clear everything
	allActionLists.clear();
}

const std::string_view &GameFeelEvents::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Components::GameFeelEvents);
	return className;
}

bool GameFeelEvents::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}

	allActionLists.clear();
	stream.ReadObject("Events", [this, &stream] (const std::string &eventName) {
		// FIXME: make a factory method to generate action lists, here
		//Settings settings;
		//if(!settings.Read(stream)) {
		//	LOG_WARNING("{}: Failed to read settings for event '{}'", NameClass(), eventName);
		//	return;
		//}
		//AddAction(eventName, settings);
	});
	return true;
}

void GameFeelEvents::AddAction(const std::string_view &name, const std::shared_ptr<IGameFeelAction> &eventSetting) {
	// Convert string_view
	std::string eventName{name};

	// Attempt to find the appropriate ActionList
	ActionList *list{nullptr};
	const auto &it = allActionLists.find(eventName);
	if(it != allActionLists.end()) {
		// Action list found, just add the event settings
		list = &(it->second);
	} else {
		// Create a new entry
		allActionLists.emplace(eventName, ActionList{eventName});
		list = &allActionLists.at(eventName);
	}

	// Queue the event setting
	list->actions.emplace_back(eventSetting);

	// Bind this action if already initialized
	if(isInitialized) {
		if(Parent() == nullptr) {
			LOG_ERROR("{}: Parent {} not found", NameClass(), NAMEOF(Entity));
			return;
		}

		Parent()->BindEvent(list->id, eventSetting->GetListener());
	}
}

bool GameFeelEvents::Play(const std::string_view &eventName, RassEngine::Events::EventArgs &args) const {
	// Attempt to find the event ID
	const auto &it = allActionLists.find(std::string{eventName});
	if(it == allActionLists.end()) {
		return false;
	}

	// Make sure parent is available as well
	if(Parent() == nullptr) {
		LOG_ERROR("{}: Parent {} not found", NameClass(), NAMEOF(Entity));
		return false;
	}

	// Run event
	Parent()->DispatchEntityEvent(it->second.id, args);
	return true;
}

}
