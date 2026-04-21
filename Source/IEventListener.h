// File Name:    IEventListener.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Interface definition for event listeners.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include "IEventArgs.h"

namespace RassEngine {

// forward declaration
template<IsEventArgs A>
class IEvent;

/// <summary>
/// The observer of <see cref="IEvent<T>"/>.  Also a functor!
/// </summary>
/// <typeparam name="A"></typeparam>
template<IsEventArgs A>
class IEventListener {
public:
	IEventListener() = default;
	inline virtual ~IEventListener() = default;

	/// <summary>
	/// The function that gets called on event trigger.
	/// </summary>
	/// <param name="source">the object pointer calling this trigger</param>
	/// <param name="args">event arguments</param>
	/// <returns></returns>
	virtual bool operator()(const IEvent<A> *source, const A &args) = 0;

private:
	// Delete everything else for now
	IEventListener(const IEventListener &) = delete;
	IEventListener(const IEventListener &&) noexcept = delete;
	IEventListener &operator=(const IEventListener &) = delete;
	IEventListener &operator=(const IEventListener &&) noexcept = delete;
};

}
