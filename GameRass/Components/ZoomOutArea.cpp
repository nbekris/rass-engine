// File Name:    ZoomOutArea.cpp
// Author(s):    main Boyuan Hu
// Course:       GAM541
// Project:      RASS
// Purpose:      Component triggering a camera zoom-out in a defined area.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "ZoomOutArea.h"

//#include "BulletBehavior.h"
#include "Cloneable.h"
#include "Component.h"
#include "Components/Camera.h"
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

ZoomOutArea::ZoomOutArea(): Cloneable<Component, ZoomOutArea>{}
	, onTriggerEnter{this, &ZoomOutArea::OnTriggerEnter}
  , onTriggerExit{this, &ZoomOutArea::OnTriggerExit}
{}

ZoomOutArea::ZoomOutArea(const ZoomOutArea &other)
	: Cloneable<Component, ZoomOutArea>{other}
	, onTriggerEnter{this, &ZoomOutArea::OnTriggerEnter}
	, onTriggerExit{this, &ZoomOutArea::OnTriggerExit}
	, zoomOutValue{other.zoomOutValue} {}

ZoomOutArea::~ZoomOutArea() {
	if(Parent() == nullptr) {
		return;
	}
	Parent()->UnbindEvent(Events::TriggerEventID::OnTriggerEnter, &onTriggerEnter);
	Parent()->UnbindEvent(Events::TriggerEventID::OnTriggerExit, &onTriggerExit);
}

bool ZoomOutArea::Initialize() {
	if(Parent() == nullptr) {
		LOG_ERROR("Cannot initialize {}: parent entity not found", NameClass());
		return false;
	}
	Parent()->BindEvent(Events::TriggerEventID::OnTriggerEnter, &onTriggerEnter);
	Parent()->BindEvent(Events::TriggerEventID::OnTriggerExit, &onTriggerExit);
	return true;
}

const std::string_view &ZoomOutArea::NameClass() const {
	static constexpr std::string_view className = NAMEOF(ZoomOutArea);
	return className;
}

bool ZoomOutArea::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}
	stream.Read("ZoomOutValue", zoomOutValue);
	return true;
}

bool ZoomOutArea::OnTriggerEnter(const IEvent<Events::EventArgs> *, const Events::EventArgs &args) {
	Entity *other = dynamic_cast<const Events::TriggerEventArgs &>(args).otherEntity;
	if(!other) {
		return true;
	}


	if(auto *camera = other->Get<Camera>()) {

		camera->targetZoom = camera->basicZoom * zoomOutValue;
	}
	return true;


}
bool ZoomOutArea::OnTriggerExit(const IEvent<Events::EventArgs> *, const Events::EventArgs &args) {
	Entity *other = dynamic_cast<const Events::TriggerEventArgs &>(args).otherEntity;
	if(!other) {
		return true;
	}
	if(auto *camera = other->Get<Camera>()) {
		camera->targetZoom = camera->basicZoom;
	}
	return true;
}
}
