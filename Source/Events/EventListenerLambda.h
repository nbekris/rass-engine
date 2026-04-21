// File Name:    EventListenerLambda.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Event listener using a lambda callback.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <functional>

#include "IEventArgs.h"
#include "IEventListener.h"

namespace RassEngine::Events {

/// <summary>
/// A helper implementation of <see cref="IEventListener<T>"/> for lambdas.
/// </summary>
/// <typeparam name="A">Event argument type</typeparam>
template<IsEventArgs A>
class EventListenerLambda : public IEventListener<A> {
public:
	using Type = std::function<bool(const IEvent<A> *, const A&)>;

	/// <summary>
	/// Constructs a simple event listener functor
	/// </summary>
	/// <param name="object"></param>
	/// <param name="method"></param>
	inline EventListenerLambda(Type lambda) : lambda{lambda} {}
	inline virtual ~EventListenerLambda() = default;

	/// <summary>
	/// The function that gets called on event trigger.
	/// </summary>
	/// <param name="args"></param>
	/// <returns></returns>
	inline bool operator()(const IEvent<A> *caller, const A &args) override {
		return lambda(caller, args);
	}

private:
	// Delete everything else for now
	EventListenerLambda() = delete;
	EventListenerLambda(const EventListenerLambda &) = delete;
	EventListenerLambda(const EventListenerLambda &&) noexcept = delete;
	EventListenerLambda &operator=(const EventListenerLambda &) = delete;
	EventListenerLambda &operator=(const EventListenerLambda &&) noexcept = delete;

	// Member Variables
	const Type lambda;
};

}
