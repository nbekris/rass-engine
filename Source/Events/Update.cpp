// File Name:    Update.cpp
// Author(s):    main Taro Omiya, secondary Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      Per-frame update event for game logic.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Events/Update.h"

#include "Events/GlobalEventID.h"
#include "Utils.h"

namespace RassEngine::Events {

const GlobalEventID Update::Before{NAMEOF(Update::Before)};
const GlobalEventID Update::On{NAMEOF(Update::On)};
const GlobalEventID Update::After{NAMEOF(Update::After)};

}
