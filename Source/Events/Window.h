// File Name:    Window.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Window creation and resize event definitions.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

namespace RassEngine::Events {
// forward declaration
class GlobalEventID;

// Taro's notes: do *not* change this struct into a namespace
// UUIDs get randomized if namespace
struct Window {
	static const GlobalEventID Minimize;
	static const GlobalEventID LoseFocus;
};

}
