// File Name:    Pushable.h
// Author(s):    main Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component marking an entity as pushable by the player.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string_view>

#include "Component.h"
#include "Cloneable.h"
#include "Events/EventArgs.h"
#include "Events/EventListenerMethod.h"
#include "IEvent.h"

namespace RassEngine::Components {

class Pushable : public Cloneable<Component, Pushable> {
public:
	Pushable();
	Pushable(const Pushable &other);
	virtual ~Pushable() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(Stream &stream) override;

	float GetSpeedMultiplier() const { return speedMultiplier; }

private:
	bool OnTriggerEnter(const IEvent<Events::EventArgs> *, const Events::EventArgs &);
	bool OnTriggerExit(const IEvent<Events::EventArgs> *, const Events::EventArgs &);

	Events::EventListenerMethod<Pushable, Events::EventArgs> onTriggerEnter;
	Events::EventListenerMethod<Pushable, Events::EventArgs> onTriggerExit;

	float speedMultiplier{0.5f};
};

}
