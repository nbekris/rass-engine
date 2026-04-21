// File Name:    SceneChange.cpp
// Author(s):    main Taro Omiya, secondary Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      Event signaling a scene change request.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Events/SceneChange.h"

#include "Events/GlobalEventID.h"
#include "Utils.h"

namespace RassEngine::Events {

const GlobalEventID SceneChange::BeforeInitialize{NAMEOF(SceneChange::BeforeInitialize)};
const GlobalEventID SceneChange::AfterInitialize{NAMEOF(SceneChange::AfterInitialize)};
const GlobalEventID SceneChange::BeforeShutdown{NAMEOF(SceneChange::BeforeShutdown)};
const GlobalEventID SceneChange::AfterShutdown{NAMEOF(SceneChange::AfterShutdown)};

}
