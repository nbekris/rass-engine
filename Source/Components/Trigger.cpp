// File Name:    Trigger.cpp
// Author(s):    main Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component detecting overlapping entities as trigger events.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Trigger.h"

#include <glm/gtx/norm.hpp>

#include "Collider.h"
#include "Entity.h"
#include "Stream.h"

namespace RassEngine::Components {

Trigger::Trigger()
	: Cloneable<Component, Trigger>{} {}

Trigger::Trigger(const Trigger &other)
	: Cloneable<Component, Trigger>{other}
	, type{other.type}
	, size{other.size}
	, offset{other.offset}
	, radius{other.radius} {}

Trigger::~Trigger() {}

bool Trigger::Initialize() {
	return true;
}

const std::string_view &Trigger::NameClass() const {
	static constexpr std::string_view className = NAMEOF(RassEngine::Components::Trigger);
	return className;
}

bool Trigger::Read(Stream &stream) {
	if(!Component::Read(stream)) {
		return false;
	}

	std::string colliderType;
	stream.Read("ColliderType", colliderType);
	stream.ReadVec3("Size", size);
	stream.ReadVec3("Offset", offset);
	stream.Read("Radius", radius);

	if(colliderType == "Box") {
		type = Collider::ColliderType::Box;
	} else if(colliderType == "Circle") {
		type = Collider::ColliderType::Circle;
	}

	return true;
}

Collider::AABB Trigger::GetBoxBounds(const glm::vec3 &position) const {
	Collider::AABB box;
	glm::vec3 halfSize = size * 0.5f;
	glm::vec3 worldCenter = position + offset;
	box.min = worldCenter - halfSize;
	box.max = worldCenter + halfSize;
	return box;
}

Collider::Circle Trigger::GetCircleBounds(const glm::vec3 &position) const {
	Collider::Circle circle;
	circle.circleRadius = this->radius;
	circle.center = position + offset;
	return circle;
}

bool Trigger::Intersects(const Collider &other, const glm::vec3 &myPos, const glm::vec3 &otherPos) const {
	if(type == Collider::ColliderType::Box && other.type == Collider::ColliderType::Box) {
		Collider::AABB aBounds = GetBoxBounds(myPos);
		Collider::AABB bBounds = other.GetBoxBounds(otherPos);

		bool overlapX = (aBounds.min.x <= bBounds.max.x) && (aBounds.max.x >= bBounds.min.x);
		bool overlapY = (aBounds.min.y <= bBounds.max.y) && (aBounds.max.y >= bBounds.min.y);

		return overlapX && overlapY;
	}

	if(type == Collider::ColliderType::Circle && other.type == Collider::ColliderType::Circle) {
		Collider::Circle aCircle = GetCircleBounds(myPos);
		Collider::Circle bCircle = other.GetCircleBounds(otherPos);

		float r = aCircle.circleRadius + bCircle.circleRadius;
		r *= r;
		float dist = glm::distance2(aCircle.center, bCircle.center);

		return dist <= r;
	}

	return false;
}

}
