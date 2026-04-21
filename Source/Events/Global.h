// File Name:    Global.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Global event system utilities.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

namespace RassEngine::Events {
// forward declaration
class GlobalEventID;

// Taro's notes: do *not* change this struct into a namespace
// UUIDs get randomized if namespace
struct Global {
	static const GlobalEventID &Update;
	static const GlobalEventID &FixedUpdate;
	static const GlobalEventID &Render;
	static const GlobalEventID Quit;
};
}
