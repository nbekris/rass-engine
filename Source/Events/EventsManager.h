// File Name:    EventsManager.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Manager coordinating event subscription and dispatch.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once
#include "Precompiled.h"

#include <memory>
#include <string_view>
#include <unordered_map>
#include <concepts>

#include "EventSynchronous.h"
#include "IEvent.h"
#include "IEventArgs.h"
#include "IEventListener.h"
#include "Object.h"
#include "UUID.h"
#include "Utils.h"

namespace RassEngine::Events {

template <IsUUID ID, IsEventArgs ARGS, class EVENT = EventSynchronous<ARGS>>
	requires std::derived_from<EVENT, IEvent<ARGS>>
class EventsManager : Object {
public:
	inline EventsManager() : typeToEventMap{} {};
	inline virtual ~EventsManager() {
		typeToEventMap.clear();
	};

	// Inherited via Object
	inline virtual bool Initialize() override {
		return true;
	}
	const std::string_view &NameClass() const override;

	bool call(const ID &eventID, const ARGS &args);
	void bind(const ID &eventID, IEventListener<ARGS> *listener, CallFrequency frequency);
	void unbind(const ID &eventID, IEventListener<ARGS> *listener);

	// Make a helper class that stores event IDs
	class Event : IEvent<ARGS> {
	public:
		Event(const ID &eventID) : eventID{eventID}, event{} {}
		const ID &eventID;

		inline bool call(const ARGS &args) override {
			return event.call(args);
		}
		inline void bind(IEventListener<ARGS> *listener, CallFrequency frequency) override {
			event.bind(listener, frequency);
		}
		inline void unbind(IEventListener<ARGS> *listener) override {
			event.unbind(listener);
		}
	private:
		EVENT event;
	};

private:
	// Remove the rest of the default functions
	EventsManager(const EventsManager &) = delete;
	EventsManager(EventsManager &&) noexcept = delete;
	EventsManager &operator=(const EventsManager &) = delete;
	EventsManager &operator=(EventsManager &&) noexcept = delete;

	// Member variables
	std::unordered_map<UUID::Type, std::unique_ptr<Event>> typeToEventMap;
};

template <IsUUID ID, IsEventArgs ARGS, class EVENT>
	requires std::derived_from<EVENT, IEvent<ARGS>>
inline bool EventsManager<ID, ARGS, EVENT>::call(const ID &eventID, const ARGS &args) {
	// Check if the event hasn't been created yet
	const auto &it = typeToEventMap.find(static_cast<UUID::Type>(eventID));
	if(it == typeToEventMap.end()) {
		// If not, return true, immediately
		return true;
	}

	// Otherwise, call the events
	return it->second->call(args);
}

template <IsUUID ID, IsEventArgs ARGS, class EVENT>
	requires std::derived_from<EVENT, IEvent<ARGS>>
void EventsManager<ID, ARGS, EVENT>::bind(const ID &eventID, IEventListener<ARGS> *listener, CallFrequency frequency) {
	// Check if the event has been created
	const auto &it = typeToEventMap.find(static_cast<UUID::Type>(eventID));
	if(it != typeToEventMap.end()) {
		// Bind to the existing event
		it->second->bind(listener, frequency);
		return;
	}

	// Otherwise, create a new event, and bind the key and listener
	std::unique_ptr<Event> newEvent = std::make_unique<Event>(eventID);
	newEvent->bind(listener, frequency);
	typeToEventMap.emplace(static_cast<UUID::Type>(eventID), std::move(newEvent));
}

template <IsUUID ID, IsEventArgs ARGS, class EVENT>
	requires std::derived_from<EVENT, IEvent<ARGS>>
void EventsManager<ID, ARGS, EVENT>::unbind(const ID &eventID, IEventListener<ARGS> *listener) {
	// Check if the event hasn't been created yet
	const auto &it = typeToEventMap.find(eventID);
	if(it == typeToEventMap.end()) {
		// If not, halt
		return;
	}

	// Unbind the event
	it->second->unbind(listener);
}

template <IsUUID ID, IsEventArgs ARGS, class EVENT>
	requires std::derived_from<EVENT, IEvent<ARGS>>
const std::string_view &EventsManager<ID, ARGS, EVENT>::NameClass() const {
	static constexpr std::string_view className = NAMEOF(EventsManager);
	return className;
}

}
