// File Name:    Trigger.h
// Author(s):    main Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Component detecting overlapping entities as trigger events.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <glm/glm.hpp>

#include "Component.h"
#include "Cloneable.h"
#include "Components/Collider.h"

// Forward declaration
namespace RassEngine {
	class Entity;
}

namespace RassEngine::Components {
class Trigger : public Cloneable<Component, Trigger> {
public:
	Trigger();
	Trigger(const Trigger &other);
	virtual ~Trigger() override;

	virtual bool Initialize() override;
	virtual const std::string_view &NameClass() const override;
	virtual bool Read(Stream &stream) override;

	/// <summary>
	/// Check if this trigger volume overlaps with a collider at the given positions.
	/// </summary>
	bool Intersects(const Collider &other, const glm::vec3 &myPos, const glm::vec3 &otherPos) const;

	/// <summary>
	/// Get the AABB bounds for this trigger volume.
	/// </summary>
	Collider::AABB GetBoxBounds(const glm::vec3 &position) const;

	/// <summary>
	/// Get the circle bounds for this trigger volume.
	/// </summary>
	Collider::Circle GetCircleBounds(const glm::vec3 &position) const;

public:
	Collider::ColliderType type{Collider::ColliderType::Box};
	glm::vec3 size{1.0f, 1.0f, 1.0f};
	glm::vec3 offset{0.0f, 0.0f, 0.0f};
	float radius{0.5f};
};

}
