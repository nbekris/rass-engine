// File Name:    PushPullController.cpp
// Author(s):    main Niko Bekris, secondary Boyuan Hu, Taro Omiya
// Course:       GAM541
// Project:      RASS
// Purpose:      Component allowing the player to push or pull objects.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Components/PushPullController.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include "Component.h"
#include "Cloneable.h"
#include "Entity.h"
#include "Events/Global.h"
#include "Events/GlobalEventArgs.h"
#include "IEvent.h"
#include "Components/Transform.h"
#include "Components/PhysicsBody.h"
#include "Components/Movement.h"
#include "Components/Pushable.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Events/AudioEvents.h"
#include "Systems/Input/IInputSystem.h"
#include "Systems/Time/ITimeSystem.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Systems/Scene/ISceneSystem.h"
#include "InputActions.h"

namespace RassEngine::Components {

PushPullController::PushPullController()
	: Cloneable<Component, PushPullController>{}
	, onFixedUpdateListener{this, &PushPullController::OnFixedUpdate} {}

PushPullController::PushPullController(const PushPullController &other)
	: Cloneable<Component, PushPullController>{other}
	, onFixedUpdateListener{this, &PushPullController::OnFixedUpdate}
	, maxGrabDistance{other.maxGrabDistance}
	, grabAudioFile{other.grabAudioFile}
	, textEntityName{other.textEntityName} {}

PushPullController::~PushPullController() {
	if(Systems::IGlobalEventsSystem::Get() == nullptr) {
		return;
	}
	Systems::IGlobalEventsSystem::Get()->unbind(Events::Global::FixedUpdate, &onFixedUpdateListener);
}

bool PushPullController::Initialize() {
	if(Systems::IGlobalEventsSystem::Get() == nullptr) {
		LOG_ERROR("Cannot initialize {}: {} is not registered", NameClass(), NAMEOF(Systems::IGlobalEventsSystem));
		return false;
	}

	inputSystem = Systems::IInputSystem::Get();
	transform = Parent()->Get<Transform>();
	physics = Parent()->Get<PhysicsBody>();
	movement = Parent()->Get<Movement>();

	RassEngine::Events::AudioEvents::LoadSound(grabAudioFile);

	if(!transform || !physics || !movement) {
		LOG_ERROR("Cannot initialize {}: missing Transform, PhysicsBody, or Movement on entity", NameClass());
		return false;
	}

	Systems::IGlobalEventsSystem::Get()->bind(Events::Global::FixedUpdate, &onFixedUpdateListener);
	LinkTextEntity();
	return true;
}

void PushPullController::LinkTextEntity() {
	if(Systems::ISceneSystem::Get() == nullptr) {
		LOG_WARNING("{}: Cannot link text entity: {} is not registered", NameClass(), NAMEOF(Systems::ISceneSystem));
		return;
	}
	text = Systems::ISceneSystem::Get()->FindEntity(textEntityName);
	if(!text) {
		LOG_WARNING("{}: Text entity '{}' not found", NameClass(), textEntityName);
		return;
	}
	entityLinked = true;
}

const std::string_view &PushPullController::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassEngine::Components::PushPullController);
	return className;
}

bool PushPullController::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}
	stream.Read("MaxGrabDistance", maxGrabDistance);
	stream.Read("GrabAudioFile", grabAudioFile);
	stream.Read("TextEntityName", textEntityName);
	return true;
}

void PushPullController::AddNearbyPushable(Entity *entity) {
	auto it = std::find(nearbyPushables.begin(), nearbyPushables.end(), entity);
	if(it == nearbyPushables.end()) {
		nearbyPushables.push_back(entity);
	}
}

void PushPullController::RemoveNearbyPushable(Entity *entity) {
	nearbyPushables.erase(
		std::remove(nearbyPushables.begin(), nearbyPushables.end(), entity),
		nearbyPushables.end()
	);
	if(grabbedEntity == entity) {
		ReleaseGrab();
	}
}

void PushPullController::ReleaseGrab() {
	if(grabbedEntity) {
		auto *grabbedPhysics = grabbedEntity->Get<PhysicsBody>();
		if(grabbedPhysics) {
			glm::vec3 vel = grabbedPhysics->GetVelocity();
			vel.x = 0.0f;
			grabbedPhysics->SetVelocity(vel);
		}
	}
	grabbedEntity = nullptr;
	movement->isGrabbing = false;
	movement->speedMultiplierOverride = 1.0f;
}

bool PushPullController::OnFixedUpdate(const IEvent<Events::GlobalEventArgs> *, const Events::GlobalEventArgs &) {
	float dt = Systems::ITimeSystem::Get()->GetFixedDeltaTimeSec();

	// Remove destroyed entities from nearby list
	nearbyPushables.erase(
		std::remove_if(nearbyPushables.begin(), nearbyPushables.end(),
			[] (Entity *e) { return e->IsDestroyed(); }),
		nearbyPushables.end()
	);

	// Release if grabbed entity was destroyed
	if(grabbedEntity && grabbedEntity->IsDestroyed()) {
		grabbedEntity = nullptr;
		movement->isGrabbing = false;
		movement->speedMultiplierOverride = 1.0f;
	}
	//Show Text Tips
	if(!nearbyPushables.empty() && !InputActions::IsGrabBoxHeld()) {
		if(entityLinked) {
			if(text != nullptr) {
				text->GetTransform()->SetLocalPosition(Parent()->GetTransform()->GetLocalPosition() + glm::vec3(-1.0f, 0.5f, 0.5f));
			}
		}
	} else {
		if(entityLinked) {
			if(text != nullptr) {
				text->GetTransform()->SetLocalPosition(Parent()->GetTransform()->GetLocalPosition() + glm::vec3(-0.5f, 1000.0f, 0.0f));
			}
		}
	}

	// Release grab when E is released
	if(grabbedEntity && !InputActions::IsGrabBoxHeld()) {
		ReleaseGrab();
	}

	// Attempt grab when E is pressed and not currently grabbing
	if(!grabbedEntity && InputActions::IsGrabBoxHeld() && !nearbyPushables.empty()) {
		// Find the nearest pushable entity
		glm::vec3 playerPos = transform->GetPosition();
		Entity *nearest = nullptr;
		float nearestDist = std::numeric_limits<float>::max();

		for(Entity *candidate : nearbyPushables) {
			if(candidate->IsDestroyed()) continue;
			float dist = std::abs(candidate->GetTransform()->GetPosition().x - playerPos.x);
			if(dist < nearestDist) {
				nearestDist = dist;
				nearest = candidate;
			}
		}

		if(nearest) {
			grabbedEntity = nearest;
			grabOffsetX = nearest->GetTransform()->GetPosition().x - playerPos.x;

			auto *pushable = nearest->Get<Pushable>();
			movement->isGrabbing = true;
			movement->speedMultiplierOverride = pushable ? pushable->GetSpeedMultiplier() : 0.5f;
		}

		RassEngine::Events::AudioEvents::PlaySFX(grabAudioFile);
	}

	// Drive grabbed entity's velocity to follow the player
	if(grabbedEntity) {
		glm::vec3 playerPos = transform->GetPosition();
		glm::vec3 grabbedPos = grabbedEntity->GetTransform()->GetPosition();

		// Check max grab distance — release if too far
		float currentDist = std::abs(grabbedPos.x - playerPos.x);
		if(currentDist > maxGrabDistance) {
			ReleaseGrab();
			return true;
		}

		// Compute corrective velocity to maintain the grab offset
		float desiredX = playerPos.x + grabOffsetX;
		float errorX = desiredX - grabbedPos.x;

		auto *grabbedPhysics = grabbedEntity->Get<PhysicsBody>();
		if(grabbedPhysics && dt > 0.0f) {
			glm::vec3 playerVel = physics->GetVelocity();
			glm::vec3 vel = grabbedPhysics->GetVelocity();
			vel.x = playerVel.x + errorX / dt;
			grabbedPhysics->SetVelocity(vel);
		}
	}

	return true;
}

}
