// File Name:    ShowOptionsOnClick.cpp
// Author(s):    main Taro Omiya, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that opens the options menu on click.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "pch.h"
#include "ShowOptionsOnClick.h"

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

ShowOptionsOnClick::ShowOptionsOnClick() : Cloneable<Component, ShowOptionsOnClick>{}
	, onClick{this, &ShowOptionsOnClick::OnClick}
{}

ShowOptionsOnClick::ShowOptionsOnClick(const ShowOptionsOnClick &other) : Cloneable<Component, ShowOptionsOnClick>{other}
	, onClick{this, &ShowOptionsOnClick::OnClick}
{}

bool ShowOptionsOnClick::Initialize() {
	if(Parent() == nullptr) {
		LOG_WARNING("{}: {} was not found", NameClass(), NAMEOF(Entity));
		return false;
	}

	Parent()->BindEvent(UIEventID::OnClick, &onClick);
	return true;
}

ShowOptionsOnClick::~ShowOptionsOnClick() {
	if(Parent() != nullptr) {
		Parent()->UnbindEvent(UIEventID::OnClick, &onClick);
	}
}

const std::string_view &ShowOptionsOnClick::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Components::ShowOptionsOnClick);
	return className;
}

bool ShowOptionsOnClick::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}

	return true;
}

bool ShowOptionsOnClick::OnClick(const IEvent<EventArgs> *, const EventArgs &) {
	if(IPauseMenuSystem::Get() == nullptr) {
		LOG_ERROR("{}: Pause menu system not found", NameClass());
		return false;
	}

	IPauseMenuSystem::Get()->ShowOptions();
	return true;
}

}
