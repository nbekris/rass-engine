// File Name:    Render.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Render event dispatched each frame for drawing.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

namespace RassEngine::Events {
// forward declaration
class GlobalEventID;

// Taro's notes: do *not* change this struct into a namespace
// UUIDs get randomized if namespace
struct Render {
	static const GlobalEventID Before;
	static const GlobalEventID On;
	static const GlobalEventID After;
};
}
