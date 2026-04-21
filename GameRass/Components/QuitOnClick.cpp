// File Name:    QuitOnClick.cpp
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that quits the application on click.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "pch.h"
#include "QuitOnClick.h"

#include <string_view>

#include <Cloneable.h>
#include <Component.h>
#include <Entity.h>
#include <Events/EventArgs.h>
#include <Events/UIEventIDs.h>
#include <IEvent.h>
#include <Stream.h>
#include <Systems/Logging/ILoggingSystem.h>
#include <Systems/Scene/ISceneSystem.h>
#include <Utils.h>

using namespace RassEngine;
using namespace RassEngine::Events;
using namespace RassEngine::Systems;

namespace RassGame::Components {

QuitOnClick::QuitOnClick() : Cloneable<Component, QuitOnClick>{}
	, onClick{this, &QuitOnClick::OnClick}
{}

QuitOnClick::QuitOnClick(const QuitOnClick &other) : Cloneable<Component, QuitOnClick>{other}
	, onClick{this, &QuitOnClick::OnClick}
{}

bool QuitOnClick::Initialize() {
	if(Parent() == nullptr) {
		LOG_WARNING("{}: {} was not found", NameClass(), NAMEOF(Entity));
		return false;
	}

	Parent()->BindEvent(UIEventID::OnClick, &onClick);
	return true;
}

QuitOnClick::~QuitOnClick() {
	if(Parent() != nullptr) {
		Parent()->UnbindEvent(UIEventID::OnClick, &onClick);
	}
}

const std::string_view &QuitOnClick::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Components::QuitOnClick);
	return className;
}

bool QuitOnClick::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}

	// Read the type
	return true;
}

bool QuitOnClick::OnClick(const IEvent<EventArgs> *, const EventArgs &) {
	if(ISceneSystem::Get() == nullptr) {
		LOG_ERROR("{}: Scene system not found when trying to change scene", NameClass());
		return false;
	}

	// Quit the game
	ISceneSystem::Get()->EndScene();
	return true;
}

}
