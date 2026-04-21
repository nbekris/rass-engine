// File Name:    EventArgs.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Base event argument data structure.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include "IEventArgs.h"

namespace RassEngine::Events {

struct EventArgs : public IEventArgs {
	inline EventArgs() : IEventArgs{} {}
	inline virtual ~EventArgs() = default;

private:
	// Remove the rest of the default functions
	EventArgs(const EventArgs &other) = delete;
	EventArgs(EventArgs &&other) noexcept = delete;
	EventArgs &operator=(const EventArgs &) = delete;
	EventArgs &operator=(EventArgs &&) noexcept = delete;
};

}
