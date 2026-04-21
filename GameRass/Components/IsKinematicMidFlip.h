// File Name:    IsKinematicMidFlip.h
// Author(s):    main Taro Omiya, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that sets an entity kinematic during a flip.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <Cloneable.h>
#include <Component.h>
#include <Components/Transform.h>
#include <Components/PhysicsBody.h>
#include <Events/EventListenerMethod.h>
#include <glm/vec3.hpp>
#include <IEvent.h>
#include <string_view>
#include <Utils.h>

#include "FlipOrigin.h"

namespace RassGame::Components {

class IsKinematicMidFlip : public RassEngine::Cloneable<RassEngine::Component, IsKinematicMidFlip> {
	static constexpr std::string_view FLIP_ORIGIN_NAME = NAMEOF(FlipOrigin);
public:
	IsKinematicMidFlip();
	IsKinematicMidFlip(const IsKinematicMidFlip &other);
	virtual ~IsKinematicMidFlip() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(RassEngine::Stream &stream) override;

private:
	bool StartFlip(const RassEngine::IEvent<FlipOrigin::FlipEventArgs> *, const FlipOrigin::FlipEventArgs &);
	bool EndFlip(const RassEngine::IEvent<FlipOrigin::FlipEventArgs> *, const FlipOrigin::FlipEventArgs &);
private:
	RassEngine::Events::EventListenerMethod<IsKinematicMidFlip, FlipOrigin::FlipEventArgs> onFlipStartListener, onFlipEndListener;
	RassEngine::Components::PhysicsBody *physicsBody{nullptr};

	// State before flip occured
	bool lastIsKinematic{false};
};

}
