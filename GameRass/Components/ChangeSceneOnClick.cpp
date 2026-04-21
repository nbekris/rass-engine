// File Name:    ChangeSceneOnClick.cpp
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that transitions to a new scene on click.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "pch.h"
#include "ChangeSceneOnClick.h"

#include <string_view>

#include <Cloneable.h>
#include <Component.h>
#include <Entity.h>
#include <Events/EventArgs.h>
#include <Events/UIEventIDs.h>
#include <IEvent.h>
#include <Scenes/SerializedScene.h>
#include <Stream.h>
#include <Systems/Logging/ILoggingSystem.h>
#include <Systems/Scene/ISceneSystem.h>
#include <Utils.h>

using namespace RassEngine;
using namespace RassEngine::Events;
using namespace RassEngine::Systems;

namespace RassGame::Components {

ChangeSceneOnClick::ChangeSceneOnClick() : Cloneable<Component, ChangeSceneOnClick>{}
	, onClick{this, &ChangeSceneOnClick::OnClick}
{}

ChangeSceneOnClick::ChangeSceneOnClick(const ChangeSceneOnClick &other) : Cloneable<Component, ChangeSceneOnClick>{other}
	, sceneName{other.sceneName}
	, onClick{this, &ChangeSceneOnClick::OnClick}
{}

bool ChangeSceneOnClick::Initialize() {
	if(Parent() == nullptr) {
		LOG_WARNING("{}: {} was not found", NameClass(), NAMEOF(Entity));
		return false;
	}

	Parent()->BindEvent(UIEventID::OnClick, &onClick);
	return true;
}

ChangeSceneOnClick::~ChangeSceneOnClick() {
	if(Parent() != nullptr) {
		Parent()->UnbindEvent(UIEventID::OnClick, &onClick);
	}
}

const std::string_view &ChangeSceneOnClick::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Components::ChangeSceneOnClick);
	return className;
}

bool ChangeSceneOnClick::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}

	// Read the type
	stream.Read("Scene", sceneName);
	return true;
}

bool ChangeSceneOnClick::OnClick(const IEvent<EventArgs> *, const EventArgs &) {
	if(ISceneSystem::Get() == nullptr) {
		LOG_ERROR("{}: Scene system not found when trying to change scene", NameClass());
		return false;
	}

	ISceneSystem::Get()->SetPendingScene<RassEngine::Scenes::SerializedScene>(sceneName);
	return true;
}

}
