// File Name:    GlobalEventsSystem.h
// Author(s):    main Taro Omiya, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      System managing global event broadcast and subscription.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string_view>

#include "Events/EventsManager.h"
#include "Events/GlobalEventArgs.h"
#include "Events/GlobalEventID.h"
#include "IEventListener.h"
#include "IGlobalEventsSystem.h"

namespace RassEngine::Systems {

class GlobalEventsSystem : public IGlobalEventsSystem {
public:
	GlobalEventsSystem();
	virtual ~GlobalEventsSystem();

	// Inherited via IGlobalEventsSystem
	bool Initialize() override;
	void Shutdown() override;
	const std::string_view &NameClass() const override;
	bool call(const Events::GlobalEventID &eventID) override;
	bool call(const Events::GlobalEventArgs &args) override;
	void bind(const Events::GlobalEventID &eventID, IEventListener<Events::GlobalEventArgs> *listener, Events::CallFrequency frequency = Events::CallFrequency::EveryCall) override;
	void unbind(const Events::GlobalEventID &eventID, IEventListener<Events::GlobalEventArgs> *listener) override;

private:
	// Remove the rest of the default functions
	GlobalEventsSystem(const GlobalEventsSystem &) = delete;
	GlobalEventsSystem(GlobalEventsSystem &&) noexcept = delete;
	GlobalEventsSystem &operator=(const GlobalEventsSystem &) = delete;
	GlobalEventsSystem &operator=(GlobalEventsSystem &&) noexcept = delete;

	Events::EventsManager<Events::GlobalEventID, Events::GlobalEventArgs> eventsManager;
};

}
