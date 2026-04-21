// File Name:    FixedUpdate.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Fixed-timestep update event for physics and logic.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

namespace RassEngine::Events {
// forward declaration
class GlobalEventID;

// Taro's notes: do *not* change this struct into a namespace
// UUIDs get randomized if namespace
struct FixedUpdate {
	static const GlobalEventID Before;
	static const GlobalEventID On;
	static const GlobalEventID After;
};
}
