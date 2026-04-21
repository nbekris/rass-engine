// File Name:    EventListenerMethod.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Event listener using a member function callback.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include "IEventArgs.h"
#include "IEventListener.h"

namespace RassEngine::Events {

/// <summary>
/// A helper implementation of <see cref="IEventListener<T>"/> for class methods.
/// </summary>
/// <typeparam name="C">Class</typeparam>
/// <typeparam name="A">Method in the class</typeparam>
template<class C, IsEventArgs A>
class EventListenerMethod : public IEventListener<A> {
public:
	using Type = bool (C:: *)(const IEvent<A> *, const A &);

	/// <summary>
	/// Constructs a simple event listener functor
	/// </summary>
	/// <param name="object"></param>
	/// <param name="method"></param>
	inline EventListenerMethod(C *object, Type method)
		: object{object}, method{method} {}
	inline virtual ~EventListenerMethod() = default;

	/// <summary>
	/// The function that gets called on event trigger.
	/// </summary>
	/// <param name="args"></param>
	/// <returns></returns>
	inline bool operator()(const IEvent<A> *caller, const A &args) override {
		return (object->*method)(caller, args);
	}

private:
	// Delete everything else for now
	EventListenerMethod() = delete;
	EventListenerMethod(const EventListenerMethod &) = delete;
	EventListenerMethod(const EventListenerMethod &&) noexcept = delete;
	EventListenerMethod &operator=(const EventListenerMethod &) = delete;
	EventListenerMethod &operator=(const EventListenerMethod &&) noexcept = delete;

	// Member Variables
	C *object;
	Type method;
};

}
