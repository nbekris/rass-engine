// File Name:    TriggerEventIDs.h
// Author(s):    main Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Event ID definitions for trigger events.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string_view>
#include "EntityEventID.h"

namespace RassEngine::Events {

// Trigger Event IDs - These are entity-specific events
class TriggerEventID {
public:
	static const EntityEventID OnTriggerEnter;
	static const EntityEventID OnTriggerStay;
	static const EntityEventID OnTriggerExit;

private:
	TriggerEventID() = delete;
};

}
