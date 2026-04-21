// File Name:    Collider.cpp
// Author(s):    main Taro Omiya, secondary Niko Bekris, Boyuan Hu, Steven Yacoub, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      Component providing collision detection geometry.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Components/Collider.h"

#include <glm/gtx/norm.hpp>

#include "Stream.h"
#include "Systems/Logging/ILoggingSystem.h"
#include "Systems/Physics/IPhysicsSystem.h"
#include "Utils.h"

namespace RassEngine::Components {
Collider::Collider() : Cloneable<Component, Collider>{}
{
}

Collider::Collider(const Collider &other)
	: Cloneable<Component, Collider>{other}
	, size{other.size}
	, offset{other.offset}
	, radius{other.radius}
	, type{other.type}
	, isTrigger{other.isTrigger}
	, isStatic{other.isStatic}
{
}

Collider::~Collider() {
	// Remove this collider from the physics system
	auto *physicsSystem = Systems::IPhysicsSystem::Get();
	if(physicsSystem && Parent()) {
		physicsSystem->UnregisterEntity(Parent());
	}
}

bool Collider::Initialize() {
	if(!Parent()) {
		LOG_ERROR("{} is missing a parent {}", NameClass(), NAMEOF(Entity));
		return false;
	}

	auto *physicsSystem = Systems::IPhysicsSystem::Get();
	if(!physicsSystem) {
		LOG_ERROR("Unable to register {} to {}", NameClass(), NAMEOF(Systems::IPhysicsSystem));
		return false;
	}

	// Register this collider
	physicsSystem->RegisterEntity(Parent());
	return true;
}

const std::string_view &Collider::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassEngine::Components::Collider);
	return className;
}

bool Collider::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}

	std::string colliderType;
	stream.Read("ColliderType", colliderType);
	stream.ReadVec3("Offset", offset);
	stream.Read("IsTrigger", isTrigger);

	if(colliderType == "Box") {
		type = ColliderType::Box;
		stream.ReadVec3("Size", size);
	} else if(colliderType == "Circle") {
		type = ColliderType::Circle;
		stream.Read("Radius", radius);
	}

	return true;
}

Collider::AABB Collider::GetBoxBounds(const glm::vec3 &position) const {
	AABB box;

	glm::vec3 halfSize = size * 0.5f;
	glm::vec3 worldCenter = position + offset;

	box.min = worldCenter - halfSize;
	box.max = worldCenter + halfSize;
	return box;
}

Collider::Circle Collider::GetCircleBounds(const glm::vec3 &position) const {
	Circle circle;

	circle.circleRadius = this->radius;
	// World Center
	circle.center = position + offset;
	return circle;
}

bool Collider::Intersects(const Collider &other, const glm::vec3 &myPos, const glm::vec3 &otherPos) const {
	if(type == ColliderType::Box && other.type == ColliderType::Box) {
		AABB aBounds = this->GetBoxBounds(myPos);
		AABB bBounds = other.GetBoxBounds(otherPos);

		bool overlapX = (aBounds.min.x <= bBounds.max.x) && (aBounds.max.x >= bBounds.min.x);
		bool overlapY = (aBounds.min.y <= bBounds.max.y) && (aBounds.max.y >= bBounds.min.y);

		return overlapX && overlapY;
	}
	// Additional collision types (e.g., Circle vs Circle, Box vs Circle) can be implemented here
	if(type == ColliderType::Circle && other.type == ColliderType::Circle) {
		Circle aCircle = this->GetCircleBounds(myPos);
		Circle bCircle = other.GetCircleBounds(otherPos);

		float r = aCircle.circleRadius + bCircle.circleRadius;
		r *= r;

		// Use distance squared to avoid sqrt for performance (glm::distance2)
		float dist = glm::distance2(aCircle.center, bCircle.center);

		return dist <= r;
	}
	return false;
}
}
