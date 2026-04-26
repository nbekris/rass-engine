//------------------------------------------------------------------------------
//
// File Name:	EmitterCone.h
// Author(s):	taro.omiya
// Course:		CS529F25
// Project:		Project 7
// Purpose:		Behavior pattern for particle emission from a cone (or circle.)
//
// Copyright © 2025 DigiPen (USA) Corporation.
//
//------------------------------------------------------------------------------

#pragma once

//------------------------------------------------------------------------------
// Includes:
//------------------------------------------------------------------------------

#include "EmitterShape.h"

//------------------------------------------------------------------------------
// External Declarations:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Namespace Declarations:
//------------------------------------------------------------------------------

namespace RassEngine::Components::Particles {
// Forward Declarations:

// Typedefs:

// Class Definition:
class EmitterCone : public EmitterShape {
	// Public Constants and Enums:
public:
	static const char *KEY;
	static const char *NAME;

	// Constructors/Destructors:
public:
	EmitterCone(void);
	EmitterCone(const EmitterCone *other);

	// All objects need a virtual destructor to have their destructor called 
	virtual ~EmitterCone(void) override;

	// Public Static Functions:
public:

	// Public Functions:
public:
	// @brief Shallow-copies this instance to a new pointer
	EmitterCone *Clone() const override {
		return new EmitterCone(this);
	}

	// @brief Read the properties of a EmitterShape component from a stream.
	//
	// @param stream = The data stream used for reading.
	void Read(Stream &stream) override;

	// @brief Selects a random location of emission
	//
	// @param transform = The center of emit-shape
	std::tuple<Vector2D, float> GetEmitTransform(const Transform *transform) const override;

	// @brief Selects a velocity for the particle
	//
	// @param transform = The center of emit-shape
	std::tuple<Vector2D, float> GetInitVelocities(const Particle &particle) const;

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
	float startAngleDeg{0.f};
	float endAngleDeg{360.f};
	float minRadius{0.f};
	float maxRadius{10.f};
};

}	// namespace
