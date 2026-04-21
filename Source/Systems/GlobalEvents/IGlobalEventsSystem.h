// File Name:    IGlobalEventsSystem.h
// Author(s):    main Taro Omiya, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      System managing global event broadcast and subscription.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include "Events/GlobalEventArgs.h"
#include "Events/GlobalEventID.h"
#include "IEventListener.h"
#include "IGlobalSystem.h"
#include "IEvent.h"

namespace RassEngine::Systems {

/// <summary>
/// System that creates events dynamically.
/// </summary>
class IGlobalEventsSystem : public IGlobalSystem<IGlobalEventsSystem> {
public:
	virtual ~IGlobalEventsSystem() = default;

	/// <summary>
	/// Calls the event binded to <c>eventID</c>.
	/// </summary>
	/// <param name="eventID">The global event to trigger</param>
	/// <returns>True if all listeners succeeded. Also returns true if event has no listeners.</returns>
	virtual bool call(const Events::GlobalEventID &eventID) = 0;

	/// <summary>
	/// Calls the event binded to <c>args.eventID</c>.
	/// </summary>
	/// <param name="args">The arguments to send to all listeners</param>
	/// <returns>True if all listeners succeeded. Also returns true if event has no listeners.</returns>
	virtual bool call(const Events::GlobalEventArgs &args) = 0;

	/// <summary>
	/// Binds a function to event, <paramref name="type"/>.
	/// Recommended: if this system is pulled from <see cref="Engine"/>, *only* bind to global events.
	/// For events where the source of the trigger is from any <see cref="IObject"/>, create an
	/// <see cref="IEvent"/> member variable (set to, say, <see cref="EventSynchronous"/>) in that object.
	/// </summary>
	/// <param name="eventID">The event to bind to</param>
	/// <param name="listener">Functor that will be called when the event is triggered</param>
	virtual void bind(const Events::GlobalEventID &eventID, IEventListener<Events::GlobalEventArgs> *listener, Events::CallFrequency frequency = Events::CallFrequency::EveryCall) = 0;

	/// <summary>
	/// Removes an object pointer, and its associated function, from this event.
	/// </summary>
	/// <param name="eventID">The event to unbind from</param>
	/// <param name="listener">Functor that will be called when the event is triggered</param>
	virtual void unbind(const Events::GlobalEventID &eventID, IEventListener<Events::GlobalEventArgs> *listener) = 0;

protected:
	// Do not allow constructing an interface
	IGlobalEventsSystem() = default;

private:
	// Remove the rest of the default functions
	IGlobalEventsSystem(const IGlobalEventsSystem &) = delete;
	IGlobalEventsSystem(IGlobalEventsSystem &&) noexcept = delete;
	IGlobalEventsSystem &operator=(const IGlobalEventsSystem &) = delete;
	IGlobalEventsSystem &operator=(IGlobalEventsSystem &&) noexcept = delete;
};

}
