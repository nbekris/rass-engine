// File Name:    PhysicsBody.h
// Author(s):    main Niko Bekris, secondary Boyuan Hu, Steven Yacoub
// Course:       GAM541
// Project:      RASS
// Purpose:      Component providing physics simulation for an entity.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <string_view>
#include <glm/glm.hpp>

#include "Component.h"
#include "Cloneable.h"

namespace RassEngine::Components {
	class PhysicsBody : public Cloneable<Component, PhysicsBody> {
		public:

			//binary collision args (for tilemap)
			bool isGrounded = false;
			bool isTouchingCeiling = false;
			bool isTouchingLeftWall = false;
			bool isTouchingRightWall = false;
			//store pos at last frame for collision resolution
			glm::vec3 previousPosition{0.0f};
			void UpdatePreviousPosition(glm::vec3 position) {
				previousPosition = position;
			}
		public:
			PhysicsBody();
			PhysicsBody(const PhysicsBody &other);

			virtual ~PhysicsBody() override;
			virtual bool Initialize() override;
			virtual const std::string_view &NameClass() const override;
			virtual bool Read(Stream &stream) override;

			inline glm::vec3 &GetVelocity() { return velocity; };
			inline const glm::vec3 &GetAcceleration() const { return acceleration; };
			inline const float GetDrag() const { return drag; };
			inline const float GetGravityScale() const { return gravityScale; }
			inline const float GetMass() const { return mass; }
			inline const float GetInverseMass() const { return inverseMass; }
			inline const float GetRestitution() const {return restitution; }

			inline void SetVelocity(const glm::vec3 &newVelocity) { this->velocity = newVelocity; };
			inline void SetAcceleration(const glm::vec3 &newAcceleration) { this->acceleration = newAcceleration; };
			void SetInverseMass(float mass);
			void AddImpulse(const glm::vec3 &impulse) {
				glm::vec3 deltaV = impulse * inverseMass;
				// Y goes directly into velocity so gravity/drag in IntegrateBodies applies naturally
				velocity.y += deltaV.y;
				// X goes into impulseVelocity; Movement reads and decays it each frame
				impulseVelocity.x += deltaV.x;
			}
		public:
			// Horizontal physics impulse (recoil, etc.), decayed and applied by Movement.
			// Avoids conflict with Movement's direct velocity.x override.
			glm::vec3 impulseVelocity{0.0f, 0.0f, 0.0f};
			glm::vec3 velocity{0.0f, 0.0f, 0.0f};
			glm::vec3 acceleration{0.0f, 0.0f, 0.0f};

			float mass{1.0f};
			float inverseMass{1.0f};
			float drag{0.9f};
			float gravityScale{1.0f};
			float restitution{0.0f};
			float friction{0.3f};

			bool useGravity{true};
			bool isKinematic{false};
	};
}


