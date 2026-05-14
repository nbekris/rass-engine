//------------------------------------------------------------------------------
//
// File Name:	Particle.h
// Author(s):	dschilling
// Course:		CS529F25
// Project:		Project 2
// Purpose:		Template class for a new object.
//
// Copyright © 2025 DigiPen (USA) Corporation.
//
//------------------------------------------------------------------------------

#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace RassEngine::Components::Particles {

struct Particle {
	// The remaining "lifetime" of the particle.
	//   Three "states":
	//	   > 0.0f: Active (Currently in use)
	//	   = 0.0f: Free (Can be allocated)
	//	   < 0.0f: Dead (Cannot be allocated)
	float lifetime{0};
	inline bool IsActive() const {
		return lifetime > std::numeric_limits<float>::epsilon();
	}
	inline bool IsDead() const {
		return lifetime < std::numeric_limits<float>::epsilon();
	}
	inline bool IsFree() const {
		return !IsActive() && !IsDead();
	}

	// The 3 "transform" values.
	glm::vec3 position{};
	glm::vec3 scale{};
	float rotationRad{0};

	// Color values
	glm::vec4 color{1.f, 1.f, 1.f, 1.f};
};

}	// namespace
