// File Name:    ResumeOnClick.cpp
// Author(s):    main Taro Omiya, secondary Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that resumes gameplay from pause on click.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "pch.h"
#include "ResumeOnClick.h"

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

ResumeOnClick::ResumeOnClick() : Cloneable<Component, ResumeOnClick>{}
	, onClick{this, &ResumeOnClick::OnClick}
{}

ResumeOnClick::ResumeOnClick(const ResumeOnClick &other) : Cloneable<Component, ResumeOnClick>{other}
	, onClick{this, &ResumeOnClick::OnClick}
{}

bool ResumeOnClick::Initialize() {
	if(Parent() == nullptr) {
		LOG_WARNING("{}: {} was not found", NameClass(), NAMEOF(Entity));
		return false;
	}

	Parent()->BindEvent(UIEventID::OnClick, &onClick);
	return true;
}

ResumeOnClick::~ResumeOnClick() {
	if(Parent() != nullptr) {
		Parent()->UnbindEvent(UIEventID::OnClick, &onClick);
	}
}

const std::string_view &ResumeOnClick::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Components::ResumeOnClick);
	return className;
}

bool ResumeOnClick::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}

	return true;
}

bool ResumeOnClick::OnClick(const IEvent<EventArgs> *, const EventArgs &) {
	if(IPauseMenuSystem::Get() == nullptr) {
		LOG_ERROR("{}: Pause menu system not found", NameClass());
		return false;
	}

	IPauseMenuSystem::Get()->Resume();
	return true;
}

}
