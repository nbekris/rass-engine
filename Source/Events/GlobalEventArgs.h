// File Name:    GlobalEventArgs.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Event argument data for global events.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include "IEventArgs.h"

namespace RassEngine::Events {
// forward declaration
class GlobalEventID;

struct GlobalEventArgs : public IEventArgs {
	inline GlobalEventArgs(const GlobalEventID &_eventID)
		: IEventArgs{}, eventID{_eventID} {}
	inline virtual ~GlobalEventArgs() = default;

	// Member variables
	const GlobalEventID& eventID;

private:
	// Remove the rest of the default functions
	GlobalEventArgs() = delete;
	GlobalEventArgs(const GlobalEventArgs &other) = delete;
	GlobalEventArgs(GlobalEventArgs &&other) noexcept = delete;
	GlobalEventArgs &operator=(const GlobalEventArgs &) = delete;
	GlobalEventArgs &operator=(GlobalEventArgs &&) noexcept = delete;
};

}
