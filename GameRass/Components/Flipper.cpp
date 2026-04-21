// File Name:    Flipper.cpp
// Author(s):    main Niko Bekris, secondary Boyuan Hu, Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Component executing the gravity-flip mechanic.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "pch.h"
#include "Flipper.h"

#include <Cloneable.h>
#include <Component.h>
#include <Entity.h>
#include <Events/Global.h>
#include <Events/TriggerEventArgs.h>
#include <Events/TriggerEventIDs.h>
#include <Events/AudioEvents.h>
#include <IEvent.h>
#include <string>
#include <string_view>
#include <Systems/GlobalEvents/IGlobalEventsSystem.h>
#include <Systems/Input/IInputSystem.h>
#include <Systems/Logging/ILoggingSystem.h>
#include <Systems/Scene/ISceneSystem.h>
#include <Utils.h>
#include <InputActions.h>

#include "Components/FlipOrigin.h"

using namespace RassEngine;
using namespace RassEngine::Components;
using namespace RassEngine::Events;
using namespace RassEngine::Systems;

namespace RassGame::Components {
static const std::string KEY_TYPE = RemoveNamespace(NAMEOF(Flipper::Type));
static constexpr std::string_view KEY_TRIGGERED_BY = "TriggeredBy";
static const std::string TYPE_BOTH = RemoveNamespace(NAMEOF(Flipper::Type::Both));
static const std::string TYPE_UPSIDE_DOWN = RemoveNamespace(NAMEOF(Flipper::Type::UpsideDown));
static const std::string TYPE_RIGHTSIDE_UP = RemoveNamespace(NAMEOF(Flipper::Type::RightSideUp));

Flipper::Flipper() : Cloneable<Component, Flipper>{}
, onTriggerEnter{this, &Flipper::OnTriggerEnter}
, onTriggerExit{this, &Flipper::OnTriggerExit}
, onUpdateListener{this, &Flipper::OnUpdate}
{ }

Flipper::Flipper(const Flipper &other) : Cloneable<Component, Flipper>{other}
, type{other.type}, triggeredBy{other.triggeredBy}
, flipAudioName{other.flipAudioName}
, textEntityName{other.textEntityName}
, onTriggerEnter{this, &Flipper::OnTriggerEnter}
, onTriggerExit{this, &Flipper::OnTriggerExit}
, onUpdateListener{this, &Flipper::OnUpdate}
{ }

bool Flipper::Initialize() {
	if(Parent() == nullptr) {
		LOG_WARNING("{}: Entity was not found", NameClass());
		return false;
	}

	Parent()->BindEvent(Events::TriggerEventID::OnTriggerEnter, &onTriggerEnter);
	Parent()->BindEvent(Events::TriggerEventID::OnTriggerExit, &onTriggerExit);

	RassEngine::Events::AudioEvents::LoadSound(flipAudioName);

	if(IGlobalEventsSystem::Get() == nullptr) {
		LOG_ERROR("Cannot initialize {}: {} is not registered", NameClass(), NAMEOF(IGlobalEventsSystem));
		return false;
	}
	IGlobalEventsSystem::Get()->bind(Global::Update, &onUpdateListener);

	inputSystem = IInputSystem::Get();

	// Retrieve FlipOrigin
	if(ISceneSystem::Get() == nullptr) {
		LOG_WARNING("{}: {} is not registered", NameClass(), NAMEOF(ISceneSystem));
		return false;
	}

	Entity *flipEntity = ISceneSystem::Get()->FindEntity(FLIP_ENTITY_NAME);
	if(flipEntity == nullptr) {
		LOG_WARNING("{}: {} with name '{}' was not found", NameClass(), NAMEOF(Entity), FLIP_ENTITY_NAME);
		return false;
	}

	flipOrigin = flipEntity->Get<FlipOrigin>();
	if(flipOrigin == nullptr) {
		LOG_WARNING("{}: {} {} was not attached to {} '{}'", NameClass(), NAMEOF(Component), NAMEOF(FlipOrigin), NAMEOF(Entity), FLIP_ENTITY_NAME);
		return false;
	}
	LinkTextEntity();
	return true;
}

Flipper::~Flipper() {
	if(Parent() == nullptr) {
		LOG_WARNING("{}: Entity was not found", NameClass());
		return;
	}

	Parent()->UnbindEvent(Events::TriggerEventID::OnTriggerEnter, &onTriggerEnter);
	Parent()->UnbindEvent(Events::TriggerEventID::OnTriggerExit, &onTriggerExit);

	if(IGlobalEventsSystem::Get() != nullptr) {
		IGlobalEventsSystem::Get()->unbind(Global::Update, &onUpdateListener);
	}
}

const std::string_view &Flipper::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Components::Flipper);
	return className;
}

bool Flipper::Read(RassEngine::Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}

	stream.Read(KEY_TRIGGERED_BY, triggeredBy);
	stream.Read("FlipAudioName", flipAudioName);
	stream.Read("TextEntityName", textEntityName);
	std::string typeString;
	if(stream.Read(KEY_TYPE, typeString)) {
		if(typeString == TYPE_BOTH) {
			type = Type::Both;
		} else if(typeString == TYPE_UPSIDE_DOWN) {
			type = Type::UpsideDown;
		} else if(typeString == TYPE_RIGHTSIDE_UP) {
			type = Type::RightSideUp;
		} else {
			LOG_WARNING("{}: Unrecognized type '{}'", NameClass(), typeString);
			return false;
		}
	}
	return true;
}

bool Flipper::OnTriggerEnter(const IEvent<EventArgs> *, const EventArgs &args) {
	if(!IsEnabled()) {
		return true;
	}

	const Entity *otherEntity = dynamic_cast<const TriggerEventArgs &>(args).otherEntity;
	if((otherEntity == nullptr) || !otherEntity->IsNamed(triggeredBy)) {
		return true;
	}

	// Check flip origin state
	bool flipped = false;
	playerInZone = true;
	if(entitiesLinked_) {
		if(text != nullptr) {
			//if(((type == Type::UpsideDown) && flipOrigin->IsRightSideUp())||((type == Type::RightSideUp) && !flipOrigin->IsRightSideUp()))
			text->GetTransform()->SetLocalPosition(Parent()->GetTransform()->GetLocalPosition() + glm::vec3(-0.5f, 0.2f, 0.5f));
		}
	}
	return true;
}
bool Flipper::OnTriggerExit(const IEvent<EventArgs> *, const EventArgs &args) {
	if(!IsEnabled()) {
		return true;
	}

	const Entity *otherEntity = dynamic_cast<const TriggerEventArgs &>(args).otherEntity;
	if((otherEntity == nullptr) || !otherEntity->IsNamed(triggeredBy)) {
		return true;
	}

	playerInZone = false;
	if(entitiesLinked_) {
		if(text != nullptr) {
			text->GetTransform()->SetLocalPosition(Parent()->GetTransform()->GetLocalPosition() + glm::vec3(0.0f, 1000.0f, 0.0f));
		}
	}
	return true;
}

bool Flipper::OnUpdate(const IEvent<GlobalEventArgs> *, const GlobalEventArgs &) {
	if(!IsEnabled() || !playerInZone || !InputActions::IsFlipPressed()) {
		return true;
	}

	bool flipped = false;

	if(type == Type::Both) {
		flipOrigin->StartFlip();
		flipped = true;
	} else if((type == Type::UpsideDown) && flipOrigin->IsRightSideUp()) {
		flipOrigin->StartFlip();
		flipped = true;
	} else if((type == Type::RightSideUp) && !flipOrigin->IsRightSideUp()) {
		flipOrigin->StartFlip();
		flipped = true;
	}

	// Probably a better way of doing this but I am too tired
	if(flipped) {
		RassEngine::Events::AudioEvents::PlaySFX(flipAudioName);
	}
	return true;
}

void Flipper::LinkTextEntity() {
	if(entitiesLinked_) {
		return;
	}
	if(ISceneSystem::Get() == nullptr) {
		LOG_WARNING("{}: {} is not registered", NameClass(), NAMEOF(ISceneSystem));
		return;
	}
	text = ISceneSystem::Get()->FindEntity(textEntityName);
	if(text == nullptr) {
		LOG_WARNING("{}: {} with name '{}' was not found", NameClass(), NAMEOF(Entity), textEntityName);
		return;
	}
	entitiesLinked_ = true;
}

}
