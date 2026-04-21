// File Name:    SceneChange.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Event signaling a scene change request.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

namespace RassEngine::Events {

// forward declaration
class GlobalEventID;

/// <summary>
/// Global events for scenes.
/// </summary>
/// <remarks>
/// While normally, global events wouldn't be made for something related to
/// existing data types, a rare exception is made here because
/// it is exceedingly difficult to bind to a scene's event *before* it is loaded.
/// </remarks>
struct SceneChange {
	static const GlobalEventID BeforeInitialize;
	static const GlobalEventID AfterInitialize;
	static const GlobalEventID BeforeShutdown;
	static const GlobalEventID AfterShutdown;
};

}
