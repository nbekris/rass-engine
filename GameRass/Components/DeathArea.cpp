// File Name:    DeathArea.cpp
// Author(s):    main Boyuan Hu
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that triggers player death on contact.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "DeathArea.h"

//#include "BulletBehavior.h"
#include "Cloneable.h"
#include "Component.h"
#include "Components/Camera.h"
#include "Components/Movement.h"
#include "Entity.h"
#include "Events/EventArgs.h"
#include "Events/TriggerEventArgs.h"
#include "Events/TriggerEventIDs.h"
#include "IEvent.h"
#include "Stream.h"
#include "Systems/Logging/ILoggingSystem.h"
using namespace RassEngine;
using namespace RassEngine::Components;
using namespace RassEngine::Events;
using namespace RassEngine::Systems;
namespace RassGame::Components {

DeathArea::DeathArea(): Cloneable<Component, DeathArea>{}
	, onTriggerEnter{this, &DeathArea::OnTriggerEnter}
  , onTriggerExit{this, &DeathArea::OnTriggerExit}
{}

DeathArea::DeathArea(const DeathArea &other)
	: Cloneable<Component, DeathArea>{other}
	, onTriggerEnter{this, &DeathArea::OnTriggerEnter}
	, onTriggerExit{this, &DeathArea::OnTriggerExit}
	, DeathValue{other.DeathValue} {}

DeathArea::~DeathArea() {
	if(Parent() == nullptr) {
		return;
	}
	Parent()->UnbindEvent(Events::TriggerEventID::OnTriggerEnter, &onTriggerEnter);
	Parent()->UnbindEvent(Events::TriggerEventID::OnTriggerExit, &onTriggerExit);
}

bool DeathArea::Initialize() {
	if(Parent() == nullptr) {
		LOG_ERROR("Cannot initialize {}: parent entity not found", NameClass());
		return false;
	}
	Parent()->BindEvent(Events::TriggerEventID::OnTriggerEnter, &onTriggerEnter);
	Parent()->BindEvent(Events::TriggerEventID::OnTriggerExit, &onTriggerExit);
	return true;
}

const std::string_view &DeathArea::NameClass() const {
	static constexpr std::string_view className = NAMEOF(DeathArea);
	return className;
}

bool DeathArea::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}
	//stream.Read("DeathValue", DeathValue);
	return true;
}

bool DeathArea::OnTriggerEnter(const IEvent<Events::EventArgs> *, const Events::EventArgs &args) {
	Entity *other = dynamic_cast<const Events::TriggerEventArgs &>(args).otherEntity;
	if(!other) {
		return true;
	}


	if(auto *movement = other->Get<Movement>()) {
		movement->RespawnAtCheckpoint();
		//camera->targetZoom = camera->basicZoom * DeathValue;
	}
	return true;

}
bool DeathArea::OnTriggerExit(const IEvent<Events::EventArgs> *, const Events::EventArgs &args) {
	//Entity *other = dynamic_cast<const Events::TriggerEventArgs &>(args).otherEntity;
	//if(!other) {
	//	return true;
	//}
	//if(auto *camera = other->Get<Camera>()) {
	//	camera->targetZoom = camera->basicZoom;
	//}
	return true;
}
}
