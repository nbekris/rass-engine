// File Name:    BackToPauseOnClick.cpp
// Author(s):    main Taro Omiya, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that returns to the pause menu on click.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "pch.h"
#include "BackToPauseOnClick.h"

#include <string_view>

#include <Cloneable.h>
#include <Component.h>
#include <Entity.h>
#include <Events/EventArgs.h>
#include <Events/UIEventIDs.h>
#include <IEvent.h>
#include <Stream.h>
#include <Systems/Logging/ILoggingSystem.h>
#include <Systems/PauseMenu/IPauseMenuSystem.h>
#include <Utils.h>

using namespace RassEngine;
using namespace RassEngine::Events;
using namespace RassEngine::Systems;

namespace RassGame::Components {

BackToPauseOnClick::BackToPauseOnClick() : Cloneable<Component, BackToPauseOnClick>{}
	, onClick{this, &BackToPauseOnClick::OnClick}
{}

BackToPauseOnClick::BackToPauseOnClick(const BackToPauseOnClick &other) : Cloneable<Component, BackToPauseOnClick>{other}
	, onClick{this, &BackToPauseOnClick::OnClick}
{}

bool BackToPauseOnClick::Initialize() {
	if(Parent() == nullptr) {
		LOG_WARNING("{}: {} was not found", NameClass(), NAMEOF(Entity));
		return false;
	}

	Parent()->BindEvent(UIEventID::OnClick, &onClick);
	return true;
}

BackToPauseOnClick::~BackToPauseOnClick() {
	if(Parent() != nullptr) {
		Parent()->UnbindEvent(UIEventID::OnClick, &onClick);
	}
}

const std::string_view &BackToPauseOnClick::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Components::BackToPauseOnClick);
	return className;
}

bool BackToPauseOnClick::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}

	return true;
}

bool BackToPauseOnClick::OnClick(const IEvent<EventArgs> *, const EventArgs &) {
	if(IPauseMenuSystem::Get() == nullptr) {
		LOG_ERROR("{}: Pause menu system not found", NameClass());
		return false;
	}

	IPauseMenuSystem::Get()->BackToPause();
	return true;
}

}
