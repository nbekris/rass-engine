// File Name:    DisableTriggerMidFlip.h
// Author(s):    main Taro Omiya, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that disables a trigger during a flip animation.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <Cloneable.h>
#include <Component.h>
#include <Components/Trigger.h>
#include <Events/EventListenerMethod.h>
#include <glm/vec3.hpp>
#include <IEvent.h>
#include <string_view>
#include <Utils.h>

#include "FlipOrigin.h"

namespace RassGame::Components {

class DisableTriggerMidFlip : public RassEngine::Cloneable<RassEngine::Component, DisableTriggerMidFlip> {
	static constexpr std::string_view FLIP_ORIGIN_NAME = NAMEOF(FlipOrigin);
public:
	DisableTriggerMidFlip();
	DisableTriggerMidFlip(const DisableTriggerMidFlip &other);
	virtual ~DisableTriggerMidFlip() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(RassEngine::Stream &stream) override;

private:
	bool StartFlip(const RassEngine::IEvent<FlipOrigin::FlipEventArgs> *, const FlipOrigin::FlipEventArgs &);
	bool EndFlip(const RassEngine::IEvent<FlipOrigin::FlipEventArgs> *, const FlipOrigin::FlipEventArgs &);
private:
	RassEngine::Events::EventListenerMethod<DisableTriggerMidFlip, FlipOrigin::FlipEventArgs> onFlipStartListener, onFlipEndListener;
	RassEngine::Components::Trigger *trigger{nullptr};

	// State before flip occured
	bool lastIsEnabled{false};
};

}
