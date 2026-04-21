// File Name:    PhysicsBody.cpp
// Author(s):    main Niko Bekris, secondary Boyuan Hu, Steven Yacoub, Taro Omiya, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      Component providing physics simulation for an entity.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "Components/PhysicsBody.h"

#include "Cloneable.h"
#include "Component.h"
#include "Components/Transform.h"
#include "Entity.h"
#include "Events/Global.h"
#include "Events/GlobalEventArgs.h"
#include "IEvent.h"
#include "Stream.h"
#include "Systems/GlobalEvents/IGlobalEventsSystem.h"
#include "Systems/Time/ITimeSystem.h"
#include "Systems/Logging/ILoggingSystem.h"

namespace RassEngine::Components {
	PhysicsBody::PhysicsBody() : Cloneable<Component, PhysicsBody>{}
 {}

	PhysicsBody::PhysicsBody(const PhysicsBody &other)
		: Cloneable<Component, PhysicsBody>{other}
		, isKinematic{other.isKinematic}
		, velocity{other.velocity}
		, acceleration{other.acceleration}
		, mass{other.mass}
		, inverseMass{other.inverseMass}
		, drag{other.drag}
		, gravityScale{other.gravityScale}
		, restitution{other.restitution}
		, friction{other.friction}
		, useGravity{other.useGravity}
	{}

	PhysicsBody::~PhysicsBody() {
		if(Systems::IGlobalEventsSystem::Get() == nullptr) {
			return;
		}
	}

	bool PhysicsBody::Initialize() {
		if(Systems::IGlobalEventsSystem::Get() == nullptr) {
			if(Systems::ILoggingSystem::Get()) {
				LOG_WARNING("{}: Could not initialize IGlobal Events System", NAMEOF(PhysicsBody));
			}
			return true;
		}

		return true;
	}

	const std::string_view &PhysicsBody::NameClass() const {
		static constexpr std::string_view className = NAMEOF(RassEngine::Components::PhysicsBody);
		return className;
	}

	bool PhysicsBody::Read(Stream &stream) {
		if(!Component::Read(stream)) {
			return false;
		}

		stream.Read("IsKinematic", isKinematic);

		stream.ReadVec3("Velocity", velocity);
		stream.ReadVec3("Acceleration", acceleration);

		stream.Read("Mass", mass);
		SetInverseMass(mass);

		stream.Read("Drag", drag);

		stream.Read("GravityScale", gravityScale);
		stream.Read("UseGravity", useGravity);
		stream.Read("Friction", friction);

		return true;
	}

	void PhysicsBody::SetInverseMass(float mass) {
		if(mass == 0) {
			inverseMass = 0;
		}
		else {
			inverseMass = 1 / mass;
		}
	}
}
