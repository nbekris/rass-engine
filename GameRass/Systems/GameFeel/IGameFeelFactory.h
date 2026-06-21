// File Name:    IGameFeelFactory.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      System for flashing the screen.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <glm/vec3.hpp>

#include <TweenCurve.h>
#include <Systems/GlobalEvents/IGlobalEventsSystem.h>

namespace RassGame::Systems {
class IGameFeelFactory : public RassEngine::Systems::IGlobalSystem<IGameFeelFactory> {

public:
	virtual ~IGameFeelFactory() = default;

	virtual bool Show(const glm::vec3 &color, const RassEngine::TweenCurve &curve) = 0;
};
}
