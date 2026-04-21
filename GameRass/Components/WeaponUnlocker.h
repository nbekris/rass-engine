// File Name:    WeaponUnlocker.h
// Author(s):    main Boyuan Hu
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that unlocks a weapon upon player interaction.
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

class WeaponUnlocker : public RassEngine::Cloneable<RassEngine::Component, WeaponUnlocker> {
public:
	WeaponUnlocker();
	WeaponUnlocker(const WeaponUnlocker &other);
	virtual ~WeaponUnlocker() override;
	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(RassEngine::Stream &stream) override;

private:
	bool OnTriggerEnter(const RassEngine::IEvent<RassEngine::Events::EventArgs> *, const RassEngine::Events::EventArgs &);
	bool OnTriggerExit(const RassEngine::IEvent<RassEngine::Events::EventArgs> *, const RassEngine::Events::EventArgs &);
private:
	RassEngine::Events::EventListenerMethod<WeaponUnlocker, RassEngine::Events::EventArgs> onTriggerEnter;
	RassEngine::Events::EventListenerMethod<WeaponUnlocker, RassEngine::Events::EventArgs> onTriggerExit;
	int weaponId{0};
	bool autoSwitch{true};
};

}
