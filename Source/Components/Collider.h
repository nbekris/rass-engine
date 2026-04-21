// File Name:    Collider.h
// Author(s):    main Niko Bekris, secondary Boyuan Hu, Steven Yacoub
// Course:       GAM541
// Project:      RASS
// Purpose:      Component providing collision detection geometry.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string_view>
#include <glm/glm.hpp>

#include "Component.h"
#include "Cloneable.h"
#include "Events/GlobalEventArgs.h"
#include "Events/GlobalEventListener.h"
#include "IEvent.h"
namespace RassEngine::Components {
	class Collider : public Cloneable<Component, Collider> {
		public:
			Collider();
			Collider(const Collider &other);
			virtual ~Collider() override;
			virtual bool Initialize() override;
			virtual const std::string_view &NameClass() const override;
			virtual bool Read(Stream &stream) override;

			void SetBox(const glm::vec3 &size, const glm::vec3 &offset = glm::vec3(0.0f)) {
				type = ColliderType::Box;
				this->size = size;
				this->offset = offset;
			}

			void SetCircle(float radius, const glm::vec3 &offset = glm::vec3(0.0f)) {
				type = ColliderType::Circle;
				this->radius = radius;
				this->offset = offset;
			}

			enum class ColliderType {
				Box,
				Circle
			};

			struct AABB {
				glm::vec3 min;
				glm::vec3 max;
			};
			const glm::vec3 &GetSize() const { return size; }
			const glm::vec3 &GetOffset() const { return offset; }
			struct Circle {
				float circleRadius;
				glm::vec3 center;
			};

			bool IsTrigger() const { return isTrigger; }
			bool IsStatic() const { return isStatic; }

			AABB GetBoxBounds(const glm::vec3 &position) const;
			Circle GetCircleBounds(const glm::vec3 &position) const;
			bool Intersects(const Collider &other, const glm::vec3 &myPos, const glm::vec3 &otherPos) const;

	public:
		ColliderType type{ColliderType::Box};
		glm::vec3 size{1.0f, 1.0f, 1.0f};
		glm::vec3 offset{0.0f, 0.0f, 0.0f};

		float radius{0.5f};
		bool isTrigger{false};
		bool isStatic{true};
	};
}

