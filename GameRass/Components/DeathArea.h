// File Name:    DeathArea.h
// Author(s):    main Boyuan Hu
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that triggers player death on contact.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string_view>

#include "Component.h"
#include "Cloneable.h"
#include "Events/EventArgs.h"
#include "Events/EventListenerMethod.h"
#include "IEvent.h"
#include "Stream.h"

namespace RassGame::Components {

class DeathArea : public RassEngine::Cloneable<RassEngine::Component, DeathArea> {
public:
	DeathArea();
	DeathArea(const DeathArea &other);
	virtual ~DeathArea() override;
	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(RassEngine::Stream &stream) override;

private:
	bool OnTriggerEnter(const RassEngine::IEvent<RassEngine::Events::EventArgs> *, const RassEngine::Events::EventArgs &);
	bool OnTriggerExit(const RassEngine::IEvent<RassEngine::Events::EventArgs> *, const RassEngine::Events::EventArgs &);
private:
	RassEngine::Events::EventListenerMethod<DeathArea, RassEngine::Events::EventArgs> onTriggerEnter;
	RassEngine::Events::EventListenerMethod<DeathArea, RassEngine::Events::EventArgs> onTriggerExit;
	float DeathValue{0.2f};
};

}
