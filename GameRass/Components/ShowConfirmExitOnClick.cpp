// File Name:    ShowConfirmExitOnClick.cpp
// Author(s):    main Taro Omiya, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that shows the confirm-exit dialog on click.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "pch.h"
#include "ShowConfirmExitOnClick.h"

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

ShowConfirmExitOnClick::ShowConfirmExitOnClick() : Cloneable<Component, ShowConfirmExitOnClick>{}
	, onClick{this, &ShowConfirmExitOnClick::OnClick}
{}

ShowConfirmExitOnClick::ShowConfirmExitOnClick(const ShowConfirmExitOnClick &other) : Cloneable<Component, ShowConfirmExitOnClick>{other}
	, onClick{this, &ShowConfirmExitOnClick::OnClick}
{}

bool ShowConfirmExitOnClick::Initialize() {
	if(Parent() == nullptr) {
		LOG_WARNING("{}: {} was not found", NameClass(), NAMEOF(Entity));
		return false;
	}

	Parent()->BindEvent(UIEventID::OnClick, &onClick);
	return true;
}

ShowConfirmExitOnClick::~ShowConfirmExitOnClick() {
	if(Parent() != nullptr) {
		Parent()->UnbindEvent(UIEventID::OnClick, &onClick);
	}
}

const std::string_view &ShowConfirmExitOnClick::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Components::ShowConfirmExitOnClick);
	return className;
}

bool ShowConfirmExitOnClick::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}

	return true;
}

bool ShowConfirmExitOnClick::OnClick(const IEvent<EventArgs> *, const EventArgs &) {
	if(IPauseMenuSystem::Get() == nullptr) {
		LOG_ERROR("{}: Pause menu system not found", NameClass());
		return false;
	}

	IPauseMenuSystem::Get()->ShowConfirmExit();
	return true;
}

}
