// File Name:    ShowHowToPlayOnClick.cpp
// Author(s):    main Taro Omiya, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that shows the how-to-play screen on click.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "pch.h"
#include "ShowHowToPlayOnClick.h"

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

ShowHowToPlayOnClick::ShowHowToPlayOnClick() : Cloneable<Component, ShowHowToPlayOnClick>{}
	, onClick{this, &ShowHowToPlayOnClick::OnClick}
{}

ShowHowToPlayOnClick::ShowHowToPlayOnClick(const ShowHowToPlayOnClick &other) : Cloneable<Component, ShowHowToPlayOnClick>{other}
	, onClick{this, &ShowHowToPlayOnClick::OnClick}
{}

bool ShowHowToPlayOnClick::Initialize() {
	if(Parent() == nullptr) {
		LOG_WARNING("{}: {} was not found", NameClass(), NAMEOF(Entity));
		return false;
	}

	Parent()->BindEvent(UIEventID::OnClick, &onClick);
	return true;
}

ShowHowToPlayOnClick::~ShowHowToPlayOnClick() {
	if(Parent() != nullptr) {
		Parent()->UnbindEvent(UIEventID::OnClick, &onClick);
	}
}

const std::string_view &ShowHowToPlayOnClick::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Components::ShowHowToPlayOnClick);
	return className;
}

bool ShowHowToPlayOnClick::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}

	return true;
}

bool ShowHowToPlayOnClick::OnClick(const IEvent<EventArgs> *, const EventArgs &) {
	if(IPauseMenuSystem::Get() == nullptr) {
		LOG_ERROR("{}: Pause menu system not found", NameClass());
		return false;
	}

	IPauseMenuSystem::Get()->ShowHowToPlay();
	return true;
}

}
