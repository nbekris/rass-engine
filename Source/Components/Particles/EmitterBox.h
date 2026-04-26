//------------------------------------------------------------------------------
//
// File Name:	EmitterBox.h
// Author(s):	taro.omiya
// Course:		CS529F25
// Project:		Project 7
// Purpose:		Behavior pattern for particle emission from a box.
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

namespace CS529::Particles
{
	// Forward Declarations:

	// Typedefs:

	// Class Definition:
	class EmitterBox : public EmitterShape
	{
		// Public Constants and Enums:
	public:
		static const char* KEY;
		static const char* NAME;

		// Constructors/Destructors:
	public:
		EmitterBox(void);
		EmitterBox(const EmitterBox* other);

		// All objects need a virtual destructor to have their destructor called 
		virtual ~EmitterBox(void) override;

		// Public Static Functions:
	public:

		// Public Functions:
	public:
		// @brief Shallow-copies this instance to a new pointer
		EmitterBox* Clone() const override { return new EmitterBox(this); }

		// @brief Read the properties of a EmitterShape component from a stream.
		//
		// @param stream = The data stream used for reading.
		void Read(CS529::Stream& stream) override;

		// @brief Selects a random location of emission
		//
		// @param transform = The center of emit-shape
    std::tuple<Vector2D, float> GetEmitTransform(const Transform* transform) const override;

		// @brief Selects a velocity for the particle
		//
		// @param transform = The center of emit-shape
		std::tuple<Vector2D, float> GetInitVelocities(const Particle& particle) const;

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
		float width{ 1.f };
		float height{ 1.f };
		bool isRotationRandom{ false };
		float turnSpeedMinDeg{ 0.f };
		float turnSpeedMaxDeg{ 0.f };
	};

}	// namespace