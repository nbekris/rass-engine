// File Name:    FixedUpdate.cpp
// Author(s):    main Taro Omiya, secondary Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      Fixed-timestep update event for physics and logic.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Events/FixedUpdate.h"

#include "Events/GlobalEventID.h"
#include "Utils.h"

namespace RassEngine::Events {

const GlobalEventID FixedUpdate::Before{NAMEOF(FixedUpdate::Before)};
const GlobalEventID FixedUpdate::On{NAMEOF(FixedUpdate::On)};
const GlobalEventID FixedUpdate::After{NAMEOF(FixedUpdate::After)};

}
