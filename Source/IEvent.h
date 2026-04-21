// File Name:    IEvent.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Interface definition for events.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include "IEventArgs.h"

namespace RassEngine::Events {
enum class CallFrequency : unsigned char {
	EveryCall,
	Once
};
}

namespace RassEngine {

// forward declaration
template<IsEventArgs A>
class IEventListener;

template<IsEventArgs A>
class IEvent {
public:
	inline virtual ~IEvent() = default;

	/// <summary>
	/// Indicate an event was triggered
	/// </summary>
	virtual bool call(const A& args) = 0;

	/// <summary>
	/// Binds a function to listen to this event.
	/// </summary>
	/// <param name="listener">
	/// The function that gets called when this event gets called.
	/// </param>
	/// <param name="frequency">
	/// The frequency that the listener will be called.  If Once, the function will be unbound after the first call.
	/// </param>
	virtual void bind(IEventListener<A> *listener, Events::CallFrequency frequency = Events::CallFrequency::EveryCall) = 0;

	/// <summary>
	/// Removes an object pointer, and its associated function, from this event.
	/// </summary>
	/// <param name="listener">
	/// The function to disassociate from this event.
	/// </param>
	virtual void unbind(IEventListener<A> *listener) = 0;

protected:
	// Do not allow constructing an interface
	IEvent() = default;

private:
	// Remove the rest of the default functions
	IEvent(const IEvent &) = delete;
	IEvent(IEvent &&) noexcept = delete;
	IEvent &operator=(const IEvent &) = delete;
	IEvent &operator=(IEvent &&) noexcept = delete;
};

}
