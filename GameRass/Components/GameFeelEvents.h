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
#include <ISerializable.h>
#include <Stream.h>

namespace RassGame::Components {

class GameFeelEvents : public RassEngine::Cloneable<RassEngine::Component, GameFeelEvents> {
	// Forward declaration of the settings
	class Settings;
public:
	GameFeelEvents();
	GameFeelEvents(const GameFeelEvents &other);
	virtual ~GameFeelEvents() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(RassEngine::Stream &stream) override;

	inline bool Play(const std::string_view &eventName) const {
		auto it = eventSettings.find(std::string(eventName));
		if (it == eventSettings.end()) {
			return false;
		}
		return it->second.Play();
	}

private:
	std::unordered_map<std::string, Settings> eventSettings{};

	class Settings : public RassEngine::ISerializable<RassEngine::Stream> {
	public:
		bool Play() const;
		bool Read(RassEngine::Stream &stream);
	};
};

}
