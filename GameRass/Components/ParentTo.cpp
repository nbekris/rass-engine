// File Name:    ParentTo.cpp
// Author(s):    main Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that sets the entity's parent on game start
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "pch.h"
#include "ParentTo.h"

#include <string_view>

#include <Cloneable.h>
#include <Component.h>
#include <Components/Transform.h>
#include <Entity.h>
#include <Events/Global.h>
#include <IEvent.h>
#include <Stream.h>
#include <Systems/GlobalEvents/IGlobalEventsSystem.h>
#include <Systems/Logging/ILoggingSystem.h>
#include <Systems/Scene/ISceneSystem.h>
#include <Utils.h>

using namespace RassEngine;
using namespace RassEngine::Components;
using namespace RassEngine::Events;
using namespace RassEngine::Systems;

namespace RassGame::Components {

ParentTo::ParentTo() : Cloneable<Component, ParentTo>{}
, onStartListener{this, &ParentTo::OnStart}
{}

ParentTo::ParentTo(const ParentTo &other) : Cloneable<Component, ParentTo>{other}
, entityName{other.entityName}
, setPosition{other.setPosition}, localPosition{other.localPosition}
, setRotation{other.setRotation}, localRotation{other.localRotation}
, setScale{other.setScale}, localScale{other.localScale}
, onStartListener{this, &ParentTo::OnStart}
{}

ParentTo::~ParentTo() {
	if(IGlobalEventsSystem::Get() != nullptr) {
		IGlobalEventsSystem::Get()->unbind(Global::Update, &onStartListener);
	}
}

bool ParentTo::Initialize() {
	if(IGlobalEventsSystem::Get() == nullptr) {
		return false;
	}

	IGlobalEventsSystem::Get()->bind(Global::Update, &onStartListener, CallFrequency::Once);
	return true;
}

const std::string_view &ParentTo::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Components::ParentTo);
	return className;
}

bool ParentTo::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}

	// Read the type
	stream.Read("EntityName", entityName);
	setPosition = stream.ReadVec3("LocalPosition", localPosition);
	setRotation = stream.Read("LocalRotation", localRotation);
	setScale = stream.ReadVec3("LocalScale", localScale);
	return true;
}

bool ParentTo::OnStart(const RassEngine::IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
	// First, retrieve this entity's transform
	if(Parent() == nullptr) {
		LOG_WARNING("{}: {} was not found", NameClass(), NAMEOF(Entity));
		return false;
	}

	Transform *transform = Parent()->GetTransform();
	if(transform == nullptr) {
		LOG_WARNING("{}: {} was not found", NameClass(), NAMEOF(Transform));
		return false;
	}

	// Check if scene is available
	if(ISceneSystem::Get() == nullptr) {
		LOG_WARNING("{}: {} is not registered", NameClass(), NAMEOF(ISceneSystem));
		return false;
	}

	// Search for the entity to parent to
	const Entity *parentEntity = ISceneSystem::Get()->FindEntity(entityName);
	if(parentEntity == nullptr) {
		LOG_WARNING("{}: {} with name \"{}\" was not found", NameClass(), NAMEOF(Entity), entityName);
		return false;
	} else if(parentEntity->GetTransform() == nullptr) {
		LOG_WARNING("{}: {} \"{}\" does not have a {} componet", NameClass(), NAMEOF(Entity), entityName, NAMEOF(Transform));
		return false;
	}

	// Update this entity's parent
	transform->SetParentTransform(parentEntity->GetTransform());
	if(setPosition) {
		transform->SetLocalPosition(localPosition);
	}
	if(setRotation) {
		transform->SetLocalRotationDeg(localRotation);
	}
	if(setScale) {
		transform->SetLocalScale(localScale);
	}
	return true;
}

}
