// File Name:    EventSynchronous.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Synchronous event dispatch implementation.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <algorithm>
#include <unordered_map>
#include <vector>

#include "EventArgs.h"
#include "IEvent.h"
#include "IEventArgs.h"
#include "IEventListener.h"

namespace RassEngine::Events {

/// <summary>
/// Event that calls its listeners synchronously (order is random)
/// </summary>
/// <typeparam name="T"></typeparam>
template<IsEventArgs T = Events::EventArgs>
class EventSynchronous : public IEvent<T> {
public:
	inline EventSynchronous() : allListeners{} {};
	inline virtual ~EventSynchronous() {
		allListeners.clear();
	};

	/// <summary>
	/// Indicate an event was triggered
	/// </summary>
  bool call(const T &args) override;

	/// <summary>
	/// Binds a function to listen to this event.
	/// </summary>
	inline void bind(IEventListener<T> *listener, CallFrequency frequency) override {
		allListeners.insert_or_assign(listener, frequency);
	}

	/// <summary>
	/// Removes an object pointer, and its associated function, from this event.
	/// </summary>
	inline void unbind(IEventListener<T> *listener) override {
		allListeners.erase(listener);
	}

private:
	// Remove the rest of the default functions
	EventSynchronous(const EventSynchronous &) = delete;
	EventSynchronous(EventSynchronous &&) noexcept = delete;
	EventSynchronous &operator=(const EventSynchronous &) = delete;
	EventSynchronous &operator=(EventSynchronous &&) noexcept = delete;

	// Member variables
	std::unordered_map<IEventListener<T> *, CallFrequency> allListeners;
};

template<IsEventArgs T>
inline bool EventSynchronous<T>::call(const T &args) {
	// Track the listeners that should be removed after this call
	std::vector<IEventListener<T> *> oneTimeListeners;
	oneTimeListeners.reserve(allListeners.size());

	// Call all observers
	for(auto [listener, frequency] : allListeners) {
		// Check if this listener should be unbound after this call
		if(frequency == CallFrequency::Once) {
			oneTimeListeners.push_back(listener);
		}

		// If any of them returns false
		if(!(*listener)(this, args)) {
			// Unbind all one-time listeners
			std::for_each(oneTimeListeners.begin(), oneTimeListeners.end(), [this] (IEventListener<T> *listener) {
				unbind(listener);
			});

			// Halt immediately, and report something failed
			return false;
		}
	}

	// Unbind all one-time listeners
	std::for_each(oneTimeListeners.begin(), oneTimeListeners.end(), [this](IEventListener<T> *listener) {
		unbind(listener);
	});
	return true;
}

}
