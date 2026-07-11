// File Name:    IGameFeelAction.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Template for a game feel action.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once
#include <ISerializable.h>
#include <Stream.h>
#include "FeelContext.h"
namespace RassGame::Systems {
class IGameFeelAction : public RassEngine::ISerializable<RassEngine::Stream> {
public:
	virtual ~IGameFeelAction() = default;
	virtual void Execute(const FeelContext &ctx) = 0;   // ctx ignored by global effects
	float triggerTime{0.0f};      // seconds after Play(); baked at parse time
};
}
