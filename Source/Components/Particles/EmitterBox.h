#pragma once

#include <glm/vec3.hpp>
#include <memory>
#include <tuple>
#include <utility>

#include "Component.h"
#include "Components/Transform.h"
#include "EmitterShape.h"
#include "Particle.h"
#include "Stream.h"

namespace RassEngine::Components::Particles {

class EmitterBox : public EmitterShape {

public:
	EmitterBox(void);
	EmitterBox(const EmitterBox &other);

	// All objects need a virtual destructor to have their destructor called 
	inline virtual ~EmitterBox(void) override = default;

	// @brief Shallow-copies this instance to a new pointer
	inline virtual std::unique_ptr<Component> Clone() const override {
		return std::make_unique<EmitterBox>(static_cast<EmitterBox const &>(*this));
	}

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
	std::tuple<glm::vec3, float> GetInitVelocities(const Particle &particle) const;

	// Private Variables:
private:
	float width{1.f};
	float height{1.f};
	bool isRotationRandom{false};
	float turnSpeedMinDeg{0.f};
	float turnSpeedMaxDeg{0.f};
};

}	// namespace
