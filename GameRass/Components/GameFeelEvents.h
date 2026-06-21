// File Name:    GameFeelEvents.h
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Component controlling GameFeelEvents open and close behavior.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>

#include <Cloneable.h>
#include <Component.h>
#include <Stream.h>
#include <Events/EventArgs.h>
#include <Events/EntityEventID.h>
#include <IEventListener.h>

namespace RassGame::Components {

// FIXME: declare this class in a different file
class IGameFeelAction {
public:
	virtual RassEngine::IEventListener<RassEngine::Events::EventArgs> *GetListener() const = 0;
};

class GameFeelEvents : public RassEngine::Cloneable<RassEngine::Component, GameFeelEvents> {
	// Forward declare helper container
	struct ActionList;
public:
	GameFeelEvents();
	GameFeelEvents(const GameFeelEvents &other);
	virtual ~GameFeelEvents() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(RassEngine::Stream &stream) override;

	void AddAction(const std::string_view &eventName, const std::shared_ptr<IGameFeelAction> &eventSetting);
	bool Play(const std::string_view &eventName, RassEngine::Events::EventArgs &args) const;

private:
	bool isInitialized{false};
	std::unordered_map<std::string, ActionList> allActionLists{};

	struct ActionList {
		inline ActionList(const std::string_view &name)
			: id{std::string{name}}
		{}
		inline ActionList(const ActionList &other)
			: id{std::string{other.id.GetName()}}
			, actions{other.actions}
		{}
		inline ~ActionList() {
			actions.clear();
		}

		RassEngine::Events::EntityEventID id;
		std::vector<std::shared_ptr<IGameFeelAction>> actions{};
	};
};

}
