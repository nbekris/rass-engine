// File Name:    IsKinematicMidFlip.cpp
// Author(s):    main Niko Bekris, secondary Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Component that sets an entity kinematic during a flip.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "pch.h"
#include "IsKinematicMidFlip.h"

#include <Cloneable.h>
#include <Component.h>
#include <Components/Transform.h>
#include <Components/PhysicsBody.h>
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

IsKinematicMidFlip::IsKinematicMidFlip() : Cloneable<Component, IsKinematicMidFlip>{}
	, onFlipStartListener{this, &IsKinematicMidFlip::StartFlip}
	, onFlipEndListener{this, &IsKinematicMidFlip::EndFlip}
	, physicsBody{nullptr}, lastIsKinematic{false}
{}

IsKinematicMidFlip::IsKinematicMidFlip(const IsKinematicMidFlip &other) : Cloneable<Component, IsKinematicMidFlip>{other}
	, onFlipStartListener{this, &IsKinematicMidFlip::StartFlip}
	, onFlipEndListener{this, &IsKinematicMidFlip::EndFlip}
	, physicsBody{nullptr}, lastIsKinematic{false}
{}

IsKinematicMidFlip::~IsKinematicMidFlip() {
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

bool IsKinematicMidFlip::Initialize() {
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
	physicsBody = Parent()->Get<PhysicsBody>();
	return true;
}

const std::string_view &IsKinematicMidFlip::NameClass() const {
	static constexpr std::string_view className = NAMEOF(Components::IsKinematicMidFlip);
	return className;
}

bool IsKinematicMidFlip::Read(Stream &stream) {
	return Component::Read(stream);
}

bool IsKinematicMidFlip::StartFlip(const IEvent<FlipOrigin::FlipEventArgs> *, const FlipOrigin::FlipEventArgs &args) {
	// Setting physics body kinematic state change
	if(physicsBody) {
		lastIsKinematic = physicsBody->isKinematic;
		physicsBody->isKinematic = true;
	}
	return true;
}

bool IsKinematicMidFlip::EndFlip(const IEvent<FlipOrigin::FlipEventArgs> *, const FlipOrigin::FlipEventArgs &args) {
	// Setting physics body kinematic state change
	if(physicsBody) {
		physicsBody->isKinematic = lastIsKinematic;
	}
	return true;
}

}
