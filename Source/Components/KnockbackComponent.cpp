// File Name:    KnockbackComponent.cpp
// Author(s):    main Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component applying knockback force to an entity.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Components/KnockbackComponent.h"

#include "Component.h"
#include "Components/PhysicsBody.h"
#include "Entity.h"
#include "Events/Global.h"
#include "Events/GlobalEventArgs.h"
#include "IEvent.h"
#include "Cloneable.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Systems/Time/ITimeSystem.h"
#include "Systems/Logging/ILoggingSystem.h"

#include <algorithm>

namespace RassEngine::Components {

KnockbackComponent::KnockbackComponent()
	: Cloneable<Component, KnockbackComponent>{}
	, onFixedUpdateListener{this, &KnockbackComponent::OnFixedUpdate} {}

KnockbackComponent::KnockbackComponent(const KnockbackComponent &other)
	: Cloneable<Component, KnockbackComponent>{other}
	, onFixedUpdateListener{this, &KnockbackComponent::OnFixedUpdate}
	, duration{other.duration}
	, forceX{other.forceX}
	, forceY{other.forceY}
{
}

KnockbackComponent::~KnockbackComponent() {
	if(Systems::IGlobalEventsSystem::Get() == nullptr) {
		return;
	}
	Systems::IGlobalEventsSystem::Get()->unbind(Events::Global::FixedUpdate, &onFixedUpdateListener);
}

bool KnockbackComponent::Initialize() {
	if(Systems::IGlobalEventsSystem::Get() == nullptr) {
		LOG_ERROR("Cannot initialize {}: {} is not registered", NameClass(), NAMEOF(Systems::IGlobalEventsSystem));
		return false;
	}

	physics = Parent()->Get<PhysicsBody>();
	if(!physics) {
		LOG_ERROR("Cannot initialize {}: PhysicsBody not found on entity", NameClass());
		return false;
	}

	Systems::IGlobalEventsSystem::Get()->bind(Events::Global::FixedUpdate, &onFixedUpdateListener);
	return true;
}

const std::string_view &KnockbackComponent::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassEngine::Components::KnockbackComponent);
	return className;
}

bool KnockbackComponent::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}
	stream.Read("Duration", duration);
	stream.Read("ForceX", forceX);
	stream.Read("ForceY", forceY);
	return true;
}

void KnockbackComponent::Activate(float directionX) {
	if(!physics) {
		return;
	}
	isActive = true;
	timer = duration;
	dirX = directionX;
	physics->GetVelocity().y = forceY;
}

bool KnockbackComponent::OnFixedUpdate(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
	if(!isActive) {
		return true;
	}

	float dt = Systems::ITimeSystem::Get()->GetFixedDeltaTimeSec();
	timer -= dt;

	float slideFactor = std::max(0.0f, timer / duration);
	physics->impulseVelocity.x = dirX * forceX * slideFactor;

	if(timer <= 0.0f) {
		isActive = false;
		physics->impulseVelocity.x = 0.0f;
	}

	return true;
}

}
