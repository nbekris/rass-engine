// File Name:    GlobalEventsSystem.cpp
// Author(s):    main Taro Omiya, secondary Eric Fleegal, Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      System managing global event broadcast and subscription.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "GlobalEventsSystem.h"

#include "Events/GlobalEventArgs.h"
#include "Events/GlobalEventID.h"
#include "IEventListener.h"

namespace RassEngine::Systems {

GlobalEventsSystem::GlobalEventsSystem() {}

GlobalEventsSystem::~GlobalEventsSystem() {
	Shutdown();
}

bool GlobalEventsSystem::Initialize() {
	return eventsManager.Initialize();
}

void GlobalEventsSystem::Shutdown() {}

bool GlobalEventsSystem::call(const Events::GlobalEventID &eventID) {
	return eventsManager.call(eventID, Events::GlobalEventArgs(eventID));
}

bool GlobalEventsSystem::call(const Events::GlobalEventArgs &args) {
	return eventsManager.call(args.eventID, args);
}

void GlobalEventsSystem::bind(const Events::GlobalEventID &eventID, IEventListener<Events::GlobalEventArgs> *listener, Events::CallFrequency frequency) {
	eventsManager.bind(eventID, listener, frequency);
}

void GlobalEventsSystem::unbind(const Events::GlobalEventID &eventID, IEventListener<Events::GlobalEventArgs> *listener) {
	eventsManager.unbind(eventID, listener);
}

const std::string_view &GlobalEventsSystem::NameClass() const {
	static constexpr std::string_view className = NAMEOF(GlobalEventsSystem);
	return className;
}

}
