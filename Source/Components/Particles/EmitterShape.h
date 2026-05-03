// File Name:    EmitterShape.h
// Author(s):    main Taro Omiya, secondary Steven Yacoub, Niko Bekris, Eric Fleegal
// Course:       GAM541
// Project:      RASS
// Purpose:      File stream utilities for reading and writing data.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

#include <glm/vec3.hpp>
#include <tuple>

#include "Cloneable.h"
#include "Component.h"
#include "Components/Transform.h"
#include "Particle.h"
#include "Random.h"
#include "Stream.h"

namespace RassEngine::Components::Particles {

class EmitterShape : public Cloneable<Component, EmitterShape> {
	// Constructors/Destructors:
public:
	EmitterShape(void);
	EmitterShape(const EmitterShape &other);

	// All objects need a virtual destructor to have their destructor called 
	virtual ~EmitterShape(void) override = default;

	// Public Functions:

	// @brief Initialize the Object.
	// @brief [NOTE: Some Objects may note require this step; others might.]  
	//
	// @return bool = true if initialization successful, otherwise false.
	bool Initialize() override {
		return true;
	};

	// @brief Read the properties associated with an object from the stream.
	virtual bool Read(Stream &stream) override;

	// @brief Selects a random location of emission
	//
	// @param transform = The center of emit-shape
	virtual std::tuple<glm::vec3, float> GetEmitTransform(const Transform &transform) const = 0;

	// @brief Selects a velocity for the particle
	//
	// @param transform = The center of emit-shape
	virtual std::tuple<glm::vec3, float> GetInitVelocities(const Particle &particle) const = 0;

protected:
	inline float GetRandomSpeed() const {
		return Random::range(speedMin, speedMax);
	}
	// Private Variables:
private:

	float speedMin{1.0f};
	float speedMax{1.0f};
};

}	// namespace
