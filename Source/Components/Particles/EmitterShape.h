//------------------------------------------------------------------------------
//
// File Name:	EmitterShape.h
// Author(s):	taro.omiya
// Course:		CS529F25
// Project:		Project 7
// Purpose:		Abstract class for particle emitter's shape of emittance.
//
// Copyright © 2025 DigiPen (USA) Corporation.
//
//------------------------------------------------------------------------------

#pragma once

//------------------------------------------------------------------------------
// Includes:
//------------------------------------------------------------------------------

#include <tuple>

#include "Component.h"
#include "Transform.h"
#include "Particle.h"
#include "Stream.h"
#include "Vector2D.h"
#include "Random.h"

//------------------------------------------------------------------------------
// External Declarations:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Namespace Declarations:
//------------------------------------------------------------------------------

namespace RassEngine::Components::Particles {
// Typedefs:

// Class Definition:
class EmitterShape : public Object {
	// Public Constants and Enums:
public:

	// Constructors/Destructors:
public:
	EmitterShape(void);
	EmitterShape(const EmitterShape *other);

	// All objects need a virtual destructor to have their destructor called 
	virtual ~EmitterShape(void) override {};

	// Public Static Functions:
public:

	// Public Functions:
public:
	virtual EmitterShape *Clone() const = 0;

	// @brief Initialize the Object.
	// @brief [NOTE: Some Objects may note require this step; others might.]  
	//
	// @return bool = true if initialization successful, otherwise false.
	bool Initialize() override {
		return true;
	};

	// @brief Update the Object each frame.
	//
	// @param dt = Delta time (in seconds) of the last frame.
	void Update(float dt) override {};

	// @brief Render the Object each frame.
	// @brief [NOTE: Modern engines handle rendering in a more complicated way.]
	// @brief [NOTE: Some Objects are rendered only when debug drawing is enabled.]
	void Render() const override {};

	// @brief Read the properties associated with an object from the stream.
	virtual void Read(Stream &stream) override;

	// @brief Selects a random location of emission
	//
	// @param transform = The center of emit-shape
	virtual std::tuple<Vector2D, float> GetEmitTransform(const Transform *transform) const = 0;

	// @brief Selects a velocity for the particle
	//
	// @param transform = The center of emit-shape
	virtual std::tuple<Vector2D, float> GetInitVelocities(const Particle &particle) const = 0;

protected:
	inline float GetRandomSpeed() const {
		return Random::Range(speedMin, speedMax);
	}

	// Public Event Handlers
public:

	// Private Functions:
private:

	// Private Constants:
private:

	// Private Static Variables:
private:

	// Private Variables:
private:

	float speedMin{1.0f};
	float speedMax{1.0f};
};

}	// namespace
