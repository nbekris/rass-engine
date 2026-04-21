// File Name:    DisableTriggerMidFlip.cpp
// Author(s):    main Niko Bekris, secondary Taro Omiya, Boyuan Hu
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that disables a trigger during a flip animation.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "pch.h"
#include "DisableTriggerMidFlip.h"

#include <Cloneable.h>
#include <Component.h>
#include <Components/Transform.h>
#include <Components/Trigger.h>
#include <Entity.h>
#include <IEvent.h>
#include <Stream.h>
#include <string_view>
#include <Systems/Logging/ILoggingSystem.h>
#include <Systems/Scene/ISceneSystem.h>
#include <glm/vec3.hpp>

using namespace RassEngine;
using namespace RassEngine::Components;
using namespace RassEngine::Events;
using namespace RassEngine::Systems;

namespace RassGame::Components {

DisableTriggerMidFlip::DisableTriggerMidFlip() : Cloneable<Component, DisableTriggerMidFlip>{}
	, onFlipStartListener{this, &DisableTriggerMidFlip::StartFlip}
	, onFlipEndListener{this, &DisableTriggerMidFlip::EndFlip}
	, trigger{nullptr}, lastIsEnabled{false}
{}

DisableTriggerMidFlip::DisableTriggerMidFlip(const DisableTriggerMidFlip &other) : Cloneable<Component, DisableTriggerMidFlip>{other}
	, onFlipStartListener{this, &DisableTriggerMidFlip::StartFlip}
	, onFlipEndListener{this, &DisableTriggerMidFlip::EndFlip}
	, trigger{nullptr}, lastIsEnabled{false}
{}

DisableTriggerMidFlip::~DisableTriggerMidFlip() {
	if(ISceneSystem::Get() == nullptr) {
		return;
	}

	Entity *unbindEntity = ISceneSystem::Get()->FindEntity(FLIP_ORIGIN_NAME);
	if(unbindEntity == nullptr) {
		return;
	}

	FlipOrigin *unbindComponent = unbindEntity->Get<FlipOrigin>();
	if(unbindComponent == nullptr) {
		return;
	}

	// Unbind from events
	unbindComponent->UnbindOnFlipStart(&onFlipStartListener);
	unbindComponent->UnbindOnFlipEnd(&onFlipEndListener);
}

bool DisableTriggerMidFlip::Initialize() {
	Transform *transform = Parent()->Get<Transform>();
	if(transform == nullptr) {
		LOG_WARNING("{}: {} requires '{}' component", NameClass(), NameClass(), NAMEOF(Transform));
		return false;
	}

	if(ISceneSystem::Get() == nullptr) {
		LOG_WARNING("{}: System '{}' is not registered", NameClass(), NAMEOF(ISceneSystem));
		return false;
	}

	Entity *bindEntity = ISceneSystem::Get()->FindEntity(FLIP_ORIGIN_NAME);
	if(bindEntity == nullptr) {
		LOG_WARNING("{}: Entity '{}' was not found in scene", NameClass(), FLIP_ORIGIN_NAME);
		return false;
	}

	FlipOrigin *bindComponent = bindEntity->Get<FlipOrigin>();
	if(bindComponent == nullptr) {
		LOG_WARNING("{}: Component '{}' was not found in entity '{}'", NameClass(), NAMEOF(FlipOrigin), FLIP_ORIGIN_NAME);
		return false;
	}

	// Bind to events
	bindComponent->BindOnFlipStart(&onFlipStartListener);
	bindComponent->BindOnFlipEnd(&onFlipEndListener);

	// Cache the physics body
	trigger = Parent()->Get<Trigger>();
	return true;
}

const std::string_view &DisableTriggerMidFlip::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Components::DisableTriggerMidFlip);
	return className;
}

bool DisableTriggerMidFlip::Read(Stream &stream) {
	return Component::Read(stream);
}

bool DisableTriggerMidFlip::StartFlip(const IEvent<FlipOrigin::FlipEventArgs> *, const FlipOrigin::FlipEventArgs &args) {
	// Setting physics body kinematic state change
	if(trigger) {
		lastIsEnabled = trigger->IsEnabled();
		trigger->SetEnabled(false);
	}
	return true;
}

bool DisableTriggerMidFlip::EndFlip(const IEvent<FlipOrigin::FlipEventArgs> *, const FlipOrigin::FlipEventArgs &args) {
	// Setting physics body kinematic state change
	if(trigger) {
		trigger->SetEnabled(lastIsEnabled);
	}
	return true;
}

}
