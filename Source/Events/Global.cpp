// File Name:    Global.cpp
// Author(s):    main Taro Omiya, secondary Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      Global event system utilities.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Global.h"

#include "Events/FixedUpdate.h"
#include "Events/GlobalEventID.h"
#include "Events/Render.h"
#include "Events/Update.h"
#include "Utils.h"

namespace RassEngine::Events {

const GlobalEventID &Global::Update = Update::On;
const GlobalEventID &Global::FixedUpdate = FixedUpdate::On;
const GlobalEventID &Global::Render = Render::On;
const GlobalEventID Global::Quit{NAMEOF(Global::Quit)};

}
