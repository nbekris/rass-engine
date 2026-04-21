// File Name:    Window.cpp
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Window creation and resize event definitions.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Events/Window.h"

#include "Events/GlobalEventID.h"
#include "Utils.h"

namespace RassEngine::Events {

const GlobalEventID Window::Minimize{NAMEOF(Window::Minimize)};
const GlobalEventID Window::LoseFocus{NAMEOF(Window::LoseFocus)};

}
