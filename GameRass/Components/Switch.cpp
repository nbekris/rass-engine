// File Name:    Switch.cpp
// Author(s):    main Niko Bekris, secondary Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Component acting as a toggleable switch that controls doors.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "pch.h"
#include "Switch.h"

#include <Cloneable.h>
#include <Component.h>
#include <Entity.h>
#include <Events/EventArgs.h>
#include <Events/EventSynchronous.h>
#include <Events/TriggerEventArgs.h>
#include <Events/TriggerEventIDs.h>
#include <IEvent.h>
#include <memory>
#include <Stream.h>
#include <string>
#include <string_view>
#include <Events/AudioEvents.h>
#include <Systems/Logging/ILoggingSystem.h>
#include <Utils.h>

using namespace RassEngine;
using namespace RassEngine::Components;
using namespace RassEngine::Events;
using namespace RassEngine::Systems;

namespace RassGame::Components {

bool Switch::isToggleOn = false;
std::unique_ptr<IEvent<Switch::ToggleEventArgs>> Switch::eventOnToggleChanged
	= std::make_unique<EventSynchronous<Switch::ToggleEventArgs>>();

Switch::Switch() : Cloneable<Component, Switch>{}
	, type{Type::TriggerOnce}, isOn{false}, triggeredBy{}
	, heldEntities{}, onSwitchChanged{}
	, onTriggerEnter{this, &Switch::OnTriggerEnter}
	, onTriggerExit{this, &Switch::OnTriggerExit}
	, onToggleChanged{this, &Switch::OnToggleChanged}
	, spriteSwapper{nullptr}
{
	onSwitchChanged = std::make_unique<EventSynchronous<ToggleEventArgs>>();
}

Switch::Switch(const Switch &other) : Cloneable<Component, Switch>{other}
	, type{other.type}, isOn{other.isOn}, triggeredBy{other.triggeredBy}, switchAudioName{other.switchAudioName}
	, heldEntities{}, onSwitchChanged{}
	, onTriggerEnter{this, &Switch::OnTriggerEnter}
	, onTriggerExit{this, &Switch::OnTriggerExit}
	, onToggleChanged{this, &Switch::OnToggleChanged}
	, spriteSwapper{nullptr}
{
	onSwitchChanged = std::make_unique<EventSynchronous<ToggleEventArgs>>();
}

bool Switch::Initialize() {
	if(Parent() == nullptr) {
		LOG_WARNING("{}: Entity was not found", NameClass());
		return false;
	}

	Parent()->BindEvent(Events::TriggerEventID::OnTriggerEnter, &onTriggerEnter);
	Parent()->BindEvent(Events::TriggerEventID::OnTriggerExit, &onTriggerExit);

	if(!switchAudioName.empty()) {
		RassEngine::Events::AudioEvents::LoadSound(switchAudioName);
	}

	// If toggle, bind to global event as well
	if(GetType() == Type::Toggle) {
		isToggleOn = false;
		eventOnToggleChanged->bind(&onToggleChanged);
	}
	
	// Cache the sprite swapper for later use
	spriteSwapper = Parent()->Get<SpriteSwapper>();
	return true;
}

Switch::~Switch() {
	if(Parent() == nullptr) {
		LOG_WARNING("{}: Entity was not found", NameClass());
		return;
	}

	Parent()->UnbindEvent(Events::TriggerEventID::OnTriggerEnter, &onTriggerEnter);
	Parent()->UnbindEvent(Events::TriggerEventID::OnTriggerExit, &onTriggerExit);

	// If toggle, unbind to global event as well
	if(GetType() == Type::Toggle) {
		eventOnToggleChanged->unbind(&onToggleChanged);
	}
}

const std::string_view &Switch::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Components::Switch);
	return className;
}

bool Switch::IsOn() const {
	if(GetType() == Type::Toggle) {
		return isToggleOn;
	}
	return isOn;
}

bool Switch::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}
	// Read a list of strings
	triggeredBy.clear();
	stream.ReadArray("TriggeredBy", [this, &stream] () {
		std::string_view name;
		if(stream.ReadAs<std::string_view>(name)) {
			triggeredBy.emplace(std::string{name});
		}
		return true;
	});

	stream.Read("SwitchAudioName", switchAudioName);

	// Read the type
	std::string_view typeStr;
	if(stream.Read("Type", typeStr)) {
		if (typeStr == "TriggerOnce") {
			type = Type::TriggerOnce;
		} else if (typeStr == "Held") {
			type = Type::Held;
		} else if (typeStr == "Toggle") {
			type = Type::Toggle;
		} else {
			LOG_WARNING("{}: Unrecognized switch type '{}'", NameClass(), typeStr);
		}
	}
	return true;
}

bool Switch::OnTriggerEnter(const IEvent<Events::EventArgs> *, const Events::EventArgs &args) {
	if(!IsEnabled()) {
		return true;
	}

	// Check if we're already triggered, and if this is a trigger-once switch
	if((type == Type::TriggerOnce) && (IsOn() == true)) {
		// Halt, immediately
		return true;
	}

	// Check what caused the trigger
	const Entity *otherEntity = dynamic_cast<const TriggerEventArgs &>(args).otherEntity;
	const std::string name{otherEntity->Name()};
	if(!triggeredBy.contains(name)) {
		// If not something we recognize, halt
		return true;
	}

	// Check switch type
	if(type == Type::Held) {
		// Add the entity into this trigger
		heldEntities.emplace(otherEntity);

		// Check if there are any entities already in the trigger
		if(heldEntities.size() > 1) {
			// If so, halt early
			return true;
		}
	}

	// Change behavior based on switch type
	if(type == Type::Toggle) {
		SetIsOn(!IsOn());
	} else {
		SetIsOn(true);
	}
	return true;
}

bool Switch::OnTriggerExit(const IEvent<Events::EventArgs> *, const Events::EventArgs &args) {
	if(!IsEnabled()) {
		return true;
	}

	// Check if this is a relevant switch type
	if(type != Type::Held) {
		// If not, halt immediately
		return true;
	}

	// Check what caused the trigger
	const Entity *otherEntity = dynamic_cast<const TriggerEventArgs &>(args).otherEntity;
	const std::string name{otherEntity->Name()};
	if(!triggeredBy.contains(name)) {
		// If not something we recognize, halt
		return true;
	}

	// Add the entity into this trigger
	heldEntities.erase(otherEntity);

	// Check if there are still any entities on the trigger
	if(heldEntities.size() >= 1) {
		// If so, halt early
		return true;
	}

	// Indicate the state has changed
	SetIsOn(false);
	return true;
}

bool Switch::OnToggleChanged(const RassEngine::IEvent<Switch::ToggleEventArgs> *, const Switch::ToggleEventArgs &args) {
	// Otherwise, update the state to match the global toggle state
	onSwitchChanged->call(ToggleEventArgs{*this});

	// If available, swap the sprites
	if(spriteSwapper != nullptr) {
		spriteSwapper->SetIndex(IsOn() ? 1 : 0);
	}
	return true;
}

void Switch::SetIsOn(bool newIsOn) {
	if(IsOn() == newIsOn) {
		return;
	}

	if(!switchAudioName.empty()) {
		RassEngine::Events::AudioEvents::PlaySFX(switchAudioName);
	}

	// If this is a toggle switch, also update the global toggle state and invoke the global event
	if(GetType() == Type::Toggle) {
		isToggleOn = newIsOn;
		eventOnToggleChanged->call(ToggleEventArgs{*this});
		return;
	}

	// Set the new state and invoke the event
	isOn = newIsOn;
	onSwitchChanged->call(ToggleEventArgs{*this});

	// If available, swap the sprites
	if(spriteSwapper != nullptr) {
		spriteSwapper->SetIndex(IsOn() ? 1 : 0);
	}
}

}
