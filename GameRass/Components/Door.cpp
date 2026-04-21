// File Name:    Door.cpp
// Author(s):    main Niko Bekris, secondary Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Component controlling door open and close behavior.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "pch.h"
#include "Door.h"

#include <Cloneable.h>
#include <Component.h>
#include <Components/Collider.h>
#include <Entity.h>
#include <IEvent.h>
#include <Stream.h>
#include <string_view>
#include <Systems/Logging/ILoggingSystem.h>
#include <Systems/Scene/ISceneSystem.h>
#include <Utils.h>
#include <UUID.h>

#include "Components/SpriteSwapper.h"
#include "Components/Switch.h"

using namespace RassEngine;
using namespace RassEngine::Components;
using namespace RassEngine::Events;
using namespace RassEngine::Systems;

namespace RassGame::Components {

Door::Door() : Cloneable<Component, Door>{}
	, type{Type::Switch}, isOpen{false}, bindTo{}
	, spriteSwapper{nullptr}, collider{nullptr}, physicsBody{nullptr}
	, onSwitchChanged{this, &Door::OnSwitchChanged}
{
	bindTo.reserve(1);
}

Door::Door(const Door &other) : Cloneable<Component, Door>{other}
	, type{other.type}, isOpen{other.isOpen}, bindTo{other.bindTo}
	, spriteSwapper{nullptr}, collider{nullptr}, physicsBody{nullptr}
	, onSwitchChanged{this, &Door::OnSwitchChanged}
{}

bool Door::Initialize() {
	if(Parent() == nullptr) {
		LOG_WARNING("{}: {} was not found", NameClass(), NAMEOF(Entity));
		return false;
	}

	// Bind to the switch's event
	for(const UUID &id : bindTo) {
		Switch *switchComponent = FindSwitch(id);
		if(switchComponent) {
			switchComponent->Bind(&onSwitchChanged);
		}
	}

	// Cache the components for later use
	spriteSwapper = Parent()->Get<SpriteSwapper>();
	collider = Parent()->Get<Collider>();
	physicsBody = Parent()->Get<PhysicsBody>();
	return true;
}

Door::~Door() {
	// Unbind to the switch's event
	for(const UUID &id : bindTo) {
		Switch *switchComponent = FindSwitch(id);
		if(switchComponent) {
			switchComponent->Unbind(&onSwitchChanged);
		}
	}
}

const std::string_view &Door::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Components::Door);
	return className;
}

bool Door::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}
	// Read the type
	std::string_view typeStr;
	if(stream.Read("Type", typeStr)) {
		if (typeStr == "Switch") {
			type = Type::Switch;
		} else if (typeStr == "Key") {
			type = Type::Key;
		} else {
			LOG_WARNING("{}: Unrecognized Door type '{}'", NameClass(), typeStr);
		}
	}

	// Read the UUID
	bindTo.clear();
	stream.ReadArray("BindTo", [this, &stream] () {
		UUID::Type bindId;
		if(stream.ReadAs<UUID::Type>(bindId)) {
			bindTo.insert(bindId);
		}
	});
	if (bindTo.size() <= 0) {
		LOG_WARNING("{}: No valid {}s were read for binding", NameClass(), NAMEOF(UUID));
	}
	return true;
}

Switch *Door::FindSwitch(const RassEngine::UUID &id) const {
	// Check if scene is available
	if(ISceneSystem::Get() == nullptr) {
		LOG_WARNING("{}: {} is not registered", NameClass(), NAMEOF(ISceneSystem));
		return nullptr;
	}

	// Search for the switch to bind to
	const Entity *switchEntity = ISceneSystem::Get()->FindEntity(id);
	if(switchEntity == nullptr) {
		LOG_WARNING("{}: {} with ID {} was not found", NameClass(), NAMEOF(Entity), static_cast<UUID::Type>(id));
		return nullptr;
	}

	Switch *switchComponent = switchEntity->Get<Switch>();
	if(switchComponent == nullptr) {
		LOG_WARNING("{}: {} with ID {} did not have a {} component attached", NameClass(), NAMEOF(Entity), static_cast<UUID::Type>(id), NAMEOF(Switch));
		return nullptr;
	}
	return switchComponent;
}

void Door::SetIsOpen(bool newIsOn) {
	if(isOpen == newIsOn) {
		return;
	}

	// Set the new state and invoke the event
	isOpen = newIsOn;

	// If available, swap the sprites
	if(spriteSwapper != nullptr) {
		spriteSwapper->SetIndex(isOpen ? 1 : 0);
	}

	// If available, set the collider to be a trigger when open
	if(collider != nullptr) {
		collider->isTrigger = isOpen;
	}

	// If available, set the physics body to be disabled when open
	if (physicsBody != nullptr) {
		physicsBody->SetEnabled(!isOpen);
	}
}

bool Door::OnSwitchChanged(const RassEngine::IEvent<Switch::ToggleEventArgs> *, const Switch::ToggleEventArgs &args) {
	SetIsOpen(args.source.IsOn());
	return true;
}

}
