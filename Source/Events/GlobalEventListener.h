// File Name:    GlobalEventListener.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Listener for globally broadcast events.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include "Events/GlobalEventArgs.h"
#include "Events/EventListenerMethod.h"
#include "Events/EventListenerLambda.h"

namespace RassEngine::Events {

template<class C>
using GlobalEventListener = Events::EventListenerMethod<C, Events::GlobalEventArgs>;

using GlobalEventListenerLambda = Events::EventListenerLambda<Events::GlobalEventArgs>;

}
