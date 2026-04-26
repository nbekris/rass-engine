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

//------------------------------------------------------------------------------
// Includes:
//------------------------------------------------------------------------------

#include "Object.h"
#include "Vector2D.h"
#include "Color.h"

//------------------------------------------------------------------------------
// External Declarations:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Namespace Declarations:
//------------------------------------------------------------------------------

namespace CS529
{
	// Forward Declarations:

	// Typedefs:

	// Class Definition:
	class Particle
	{
		// Public Constants and Enums:
	public:

		// Constructors/Destructors:
	public:

		// Public Static Functions:
	public:
		inline bool IsActive() const { return lifetime > 0.f; }

		// Public Functions:
	public:

		// Public Event Handlers
	public:

		// Private Functions:
	private:

		// Private Constants:
	private:

		// Private Static Variables:
	private:

		// Public Variables:
	public:
		// The remaining "lifetime" of the particle.
		//   Three "states":
		//	   > 0.0f: Active (Currently in use)
		//	   = 0.0f: Free (Can be allocated)
		//	   < 0.0f: Dead (Cannot be allocated)
		float lifetime{ 0 };

		// The 3 "transform" values.
		Vector2D position{};
		Vector2D scale{};
		float rotationRad{ 0 };

		// Color values
		Color color = Color::White;

		// Private Variables:
	private:
	};

}	// namespace