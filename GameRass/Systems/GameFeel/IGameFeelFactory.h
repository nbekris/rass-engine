// File Name:    IGameFeelFactory.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      System for flashing the screen.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <memory>
#include <vector>
#include <glm/vec3.hpp>

#include <TweenCurve.h>
#include <Systems/GlobalEvents/IGlobalEventsSystem.h>
#include "FeelContext.h"

namespace RassGame::Systems {
class IGameFeelAction;   //forward declaration

class IGameFeelFactory : public RassEngine::Systems::IGlobalSystem<IGameFeelFactory> {
public:
	virtual ~IGameFeelFactory() = default;

	virtual bool Show(const glm::vec3 &color, const RassEngine::TweenCurve &curve) = 0;
	virtual void PlayActions(const std::vector<std::shared_ptr<IGameFeelAction>> &actions, const FeelContext &context) = 0;
	virtual void RequestHitStop(float unscaledDuration, float timeScale) = 0;
};
}
