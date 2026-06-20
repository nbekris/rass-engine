// File Name:    GameFeelEvents.h
// Author(s):    main Niko Bekris, secondary Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Component controlling GameFeelEvents open and close behavior.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

#include <Cloneable.h>
#include <Component.h>
#include <Stream.h>
#include <Events/EventArgs.h>
#include <Events/EventSynchronous.h>

namespace RassGame::Components {

class GameFeelEvents : public RassEngine::Cloneable<RassEngine::Component, GameFeelEvents> {
public:
	// Forward declaration
	struct Args;

	GameFeelEvents();
	GameFeelEvents(const GameFeelEvents &other);
	virtual ~GameFeelEvents() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(RassEngine::Stream &stream) override;

	inline bool Play(const std::string_view &eventName, const Args &args) {
		auto it = eventSettings.find(std::string(eventName));
		if (it == eventSettings.end()) {
			return false;
		}
		return it->second.call(args);
	}

	struct Args : public RassEngine::Events::EventArgs {
	public:
	};

private:
	std::unordered_map<std::string, RassEngine::Events::EventSynchronous<Args>> eventSettings{};
};

}
