// File Name:    UIEventIDs.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Event ID definitions for UI interaction events.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string_view>
#include "EntityEventID.h"

namespace RassEngine::Events {

// Trigger Event IDs - These are entity-specific events
class UIEventID {
public:
	static const EntityEventID OnClick;

private:
	UIEventID() = delete;
};

}
