#pragma once

#include <glm/vec3.hpp>
#include <utility>

#include "Cloneable.h"
#include "Components/Transform.h"
#include "EmitterShape.h"
#include "Particle.h"
#include "Stream.h"

namespace RassEngine::Components::Particles {

class EmitterCone : public EmitterShape {
public:
	// Constructors/Destructors:
	EmitterCone(void);
	EmitterCone(const EmitterCone& other);

	// All objects need a virtual destructor to have their destructor called 
	virtual ~EmitterCone(void) override = default;

	// Public Functions:

	// @brief Read the properties of a EmitterShape component from a stream.
	//
	// @param stream = The data stream used for reading.
	bool Read(Stream &stream) override;

	// @brief Selects a random location of emission
	//
	// @param transform = The center of emit-shape
	std::tuple<glm::vec3, float> GetEmitTransform(const Transform &transform) const override;

	// @brief Selects a velocity for the particle
	//
	// @param transform = The center of emit-shape
	std::tuple<glm::vec3, float> GetInitVelocities(const Particle &particle) const override;

	inline virtual std::unique_ptr<Component> Clone() const override {
		return std::make_unique<EmitterCone>(static_cast<EmitterCone const &>(*this));
	}

	// Private Variables:
private:
	float startAngleDeg{0.f};
	float endAngleDeg{360.f};
	float minRadius{0.f};
	float maxRadius{10.f};
};

}	// namespace
