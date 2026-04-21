// File Name:    TriggerEventIDs.cpp
// Author(s):    main Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Event ID definitions for trigger events.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "TriggerEventIDs.h"

namespace RassEngine::Events {

const EntityEventID TriggerEventID::OnTriggerEnter("OnTriggerEnter");
const EntityEventID TriggerEventID::OnTriggerStay("OnTriggerStay");
const EntityEventID TriggerEventID::OnTriggerExit("OnTriggerExit");

}
