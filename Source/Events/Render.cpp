// File Name:    Render.cpp
// Author(s):    main Taro Omiya, secondary Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      Render event dispatched each frame for drawing.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Render.h"

#include "Events/GlobalEventID.h"
#include "Utils.h"

namespace RassEngine::Events {

const GlobalEventID Render::Before{NAMEOF(Render::Before)};
const GlobalEventID Render::On{NAMEOF(Render::On)};
const GlobalEventID Render::After{NAMEOF(Render::After)};

}
