// File Name:    Pushable.cpp
// Author(s):    main Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component marking an entity as pushable by the player.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Components/Pushable.h"

#include "Component.h"
#include "Cloneable.h"
#include "Entity.h"
#include "Events/TriggerEventArgs.h"
#include "Events/TriggerEventIDs.h"
#include "Components/PushPullController.h"
#include "Systems/Logging/ILoggingSystem.h"

namespace RassEngine::Components {

Pushable::Pushable()
	: Cloneable<Component, Pushable>{}
	, onTriggerEnter{this, &Pushable::OnTriggerEnter}
	, onTriggerExit{this, &Pushable::OnTriggerExit} {}

Pushable::Pushable(const Pushable &other)
	: Cloneable<Component, Pushable>{other}
	, onTriggerEnter{this, &Pushable::OnTriggerEnter}
	, onTriggerExit{this, &Pushable::OnTriggerExit}
	, speedMultiplier{other.speedMultiplier} {}

Pushable::~Pushable() {
	if(Parent() == nullptr) {
		return;
	}
	Parent()->UnbindEvent(Events::TriggerEventID::OnTriggerEnter, &onTriggerEnter);
	Parent()->UnbindEvent(Events::TriggerEventID::OnTriggerExit, &onTriggerExit);
}

bool Pushable::Initialize() {
	if(Parent() == nullptr) {
		LOG_ERROR("Cannot initialize {}: parent entity not found", NameClass());
		return false;
	}
	Parent()->BindEvent(Events::TriggerEventID::OnTriggerEnter, &onTriggerEnter);
	Parent()->BindEvent(Events::TriggerEventID::OnTriggerExit, &onTriggerExit);
	return true;
}

const std::string_view &Pushable::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassEngine::Components::Pushable);
	return className;
}

bool Pushable::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}
	stream.Read("SpeedMultiplier", speedMultiplier);
	return true;
}

bool Pushable::OnTriggerEnter(const IEvent<Events::EventArgs> *, const Events::EventArgs &args) {
	Entity *other = dynamic_cast<const Events::TriggerEventArgs &>(args).otherEntity;
	if(!other) {
		return true;
	}
	if(auto *controller = other->Get<PushPullController>()) {
		controller->AddNearbyPushable(Parent());
	}
	return true;
}

bool Pushable::OnTriggerExit(const IEvent<Events::EventArgs> *, const Events::EventArgs &args) {
	Entity *other = dynamic_cast<const Events::TriggerEventArgs &>(args).otherEntity;
	if(!other) {
		return true;
	}
	if(auto *controller = other->Get<PushPullController>()) {
		controller->RemoveNearbyPushable(Parent());
	}
	return true;
}

}
