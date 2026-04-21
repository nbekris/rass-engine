// File Name:    Flippable.h
// Author(s):    main Taro Omiya, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component marking an entity as able to be flipped.
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

class Flippable : public RassEngine::Cloneable<RassEngine::Component, Flippable> {
	static constexpr std::string_view FLIP_ORIGIN_NAME = NAMEOF(FlipOrigin);
public:
	Flippable();
	Flippable(const Flippable &other);
	virtual ~Flippable() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(RassEngine::Stream &stream) override;

private:
	bool StartFlip(const RassEngine::IEvent<FlipOrigin::FlipEventArgs> *, const FlipOrigin::FlipEventArgs &);
	bool EndFlip(const RassEngine::IEvent<FlipOrigin::FlipEventArgs> *, const FlipOrigin::FlipEventArgs &);
	//bool Update(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &);
private:
	RassEngine::Events::EventListenerMethod<Flippable, FlipOrigin::FlipEventArgs> onFlipStartListener, onFlipEndListener;
	bool animateScaling{true};
	RassEngine::Components::Transform *lastTransform{nullptr};
	glm::vec3 lastLocalScale{1.f, 1.f, 1.f};
};

}
