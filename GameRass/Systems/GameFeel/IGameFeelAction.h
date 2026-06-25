// File Name:    IGameFeelAction.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Template for a game feel action.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <Events/EventArgs.h>
#include <IEventListener.h>
#include <ISerializable.h>
#include <Stream.h>

namespace RassGame::Systems {
class IGameFeelAction : public RassEngine::ISerializable<RassEngine::Stream> {
public:
	virtual RassEngine::IEventListener<RassEngine::Events::EventArgs> *GetListener() const = 0;
};
}
